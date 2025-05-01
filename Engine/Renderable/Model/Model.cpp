#include "Model.h"
#include "Engine/Timer/Timer.h"
#include <algorithm>
#include <execution>
#include <ranges>
#include <future>
#include <thread>
#include "Engine/Vulkan/VulkanContext.h"

Model::Model(std::shared_ptr<ImageManager> imageManager) : 
    imageManager(imageManager)
{
}

void Model::Load(const std::string& path)
{
    std::cout << "Async thread ImmediateQueue memory: " << Vk::VulkanContext::GetContext()->GetImmediateQueue().get() << std::endl;

    Timer timer;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        state = LoadState::Failed;
        return ;
    }

    std::cout << "-----------------------------------------\n";
    std::cout << "Assimp::Importer: " << timer.GetElapsedTime() << "\n";
    std::cout << "-----------------------------------------\n";

    this->path = path;
    this->directory = path.substr(0, path.find_last_of('/'));

    PreFetch(scene->mRootNode, scene);
    Process(scene->mRootNode, scene);
    GenerateBoundingVolume(aabbMin, aabbMax);
    UploadToGpu();

    state = LoadState::Loaded;
}

uint32_t Model::GetMeshCount()
{
    return meshCount;
}

void Model::UploadToGpu()
{
    auto device = Vk::VulkanContext::GetContext()->GetDevice();

    Renderable::UploadToGpu();
    Materialized::UploadMaterialDataToGpu();
}

void Model::PopulateSurfacePoints()
{
    surfacePoints.reserve(vertexCount);

    for (auto& vertex : vertices)
        surfacePoints.push_back(vertex.position);
}

void Model::PreFetch(aiNode* node, const aiScene* scene)
{
    Timer timer;

    std::queue<std::pair<aiNode*, glm::mat4>> queue;
    queue.push({ node, Assimp::ConvertAssimpToGlm(node->mTransformation) });

    while (!queue.empty())
    {
        auto [currentNode, parentTransform] = queue.front();
        queue.pop();

        glm::mat4 nodeTransform = Assimp::ConvertAssimpToGlm(currentNode->mTransformation);
        glm::mat4 transform = parentTransform * nodeTransform;

        for (uint32_t i = 0; i < currentNode->mNumMeshes; ++i)
        {
            aiMesh* currentMesh = scene->mMeshes[currentNode->mMeshes[i]];
           
            uint32_t meshVertexOffset = vertexCount;
            uint32_t meshVertexCount = currentMesh->mNumVertices;

            uint32_t meshIndexOffset = indexCount;
            uint32_t meshIndexCount = currentMesh->mNumFaces * 3;

            MeshProcessInfo meshProcessInfo{};
            meshProcessInfo.mesh = currentMesh;
            meshProcessInfo.meshIndex = meshCount;
            meshProcessInfo.vertexOffset = meshVertexOffset;
            meshProcessInfo.vertexCount = meshVertexCount;
            meshProcessInfo.indexOffset = meshIndexOffset;
            meshProcessInfo.indexCount = meshIndexCount;
            meshProcessInfo.transform = transform;
            meshProcessInfos.push_back(meshProcessInfo);

            vertexCount += meshVertexCount;
            indexCount += meshIndexCount;
            meshCount++;
        }

        for (unsigned int i = 0; i < currentNode->mNumChildren; ++i)
            queue.push({ currentNode->mChildren[i], transform });
    }

    vertices.resize(vertexCount);
    indices.resize(indexCount);
    materials.resize(scene->mNumMaterials);

    aabbMax = glm::vec3(std::numeric_limits<float>::lowest());
    aabbMin = glm::vec3(std::numeric_limits<float>::max());

    std::cout << "-----------------------------------------\n";
    std::cout << "PreFetch: " << timer.GetElapsedTime() << "\n";
    std::cout << "-----------------------------------------\n";
}

void Model::Process(aiNode* node, const aiScene* scene)
{
    auto futureMaterials = std::async(std::launch::async, &Model::ProcessMaterials, this, scene);
    auto futureVertices = std::async(std::launch::async, &Model::ProcessMeshVertices, this, scene);
    auto futureIndices = std::async(std::launch::async, &Model::ProcessMeshIndices, this, scene);

    futureMaterials.get();
    futureVertices.get();
    futureIndices.get();
}

void Model::ProcessMeshVertices(const aiScene* scene)
{
    Timer timer;

    std::for_each(std::execution::seq, meshProcessInfos.begin(), meshProcessInfos.end(),
        [&](const auto& meshProcessInfo) -> void {
            ProcessMeshVertex(scene, meshProcessInfo);
        }
    );

    std::cout << "-----------------------------------------\n";
    std::cout << "ProcessMeshVertices: " << timer.GetElapsedTime() << "\n";
    std::cout << "-----------------------------------------\n";
}

void Model::ProcessMeshVertex(const aiScene* scene, const MeshProcessInfo& meshProcessInfo)
{
    glm::mat4 transformIT = glm::inverse(glm::transpose(meshProcessInfo.transform));

    auto vertex_index_range = std::views::iota(0u, meshProcessInfo.vertexCount);
    std::for_each(std::execution::seq, vertex_index_range.begin(), vertex_index_range.end(),
        [&](auto vertexIndex) -> void {
            //Position
            glm::vec3 position{ 0,0,0 };
            if (meshProcessInfo.mesh->mVertices)
            {
                position = Assimp::ConvertAssimpToGlm(meshProcessInfo.mesh->mVertices[vertexIndex]);
                position = glm::vec3(meshProcessInfo.transform * glm::vec4(position, 1.0f));
            }

            //Normals
            glm::vec3 normal{ 0,0,0 };
            if (meshProcessInfo.mesh->mNormals)
            {
                normal = Assimp::ConvertAssimpToGlm(meshProcessInfo.mesh->mNormals[vertexIndex]);
                normal = glm::vec3(transformIT * glm::vec4(normal, 0));
            }

            //Tangents
            glm::vec3 tangent{ 0,0,0 };
            if (meshProcessInfo.mesh->mTangents)
            {
                tangent = Assimp::ConvertAssimpToGlm(meshProcessInfo.mesh->mTangents[vertexIndex]);
                tangent = glm::vec3(transformIT * glm::vec4(tangent, 0));
            }

            //Textures
            glm::vec2 texcoord{ 0,0 };
            if (meshProcessInfo.mesh->mTextureCoords[0] != nullptr)
            {
                texcoord.x = meshProcessInfo.mesh->mTextureCoords[0][vertexIndex].x;
                texcoord.y = meshProcessInfo.mesh->mTextureCoords[0][vertexIndex].y;
            }

            vertices[meshProcessInfo.vertexOffset + vertexIndex] = Vertex(position, normal, tangent, texcoord, meshProcessInfo.mesh->mMaterialIndex);

            aabbMax = glm::max(aabbMax, position);
            aabbMin = glm::min(aabbMin, position);
        }
    );
}

void Model::ProcessMeshIndices(const aiScene* scene)
{
    Timer timer;

    std::for_each(std::execution::seq, meshProcessInfos.begin(), meshProcessInfos.end(), 
        [&](const auto& meshProcessInfo) -> void {
            ProcessMeshIndex(scene, meshProcessInfo);
        }
    );

    std::cout << "-----------------------------------------\n";
    std::cout << "ProcessMeshIndices: " << timer.GetElapsedTime() << "\n";
    std::cout << "-----------------------------------------\n";
}

void Model::ProcessMeshIndex(const aiScene* scene, const MeshProcessInfo& meshProcessInfo)
{
    constexpr uint32_t faceIndexCount = 3;
    auto index_range = std::views::iota(0u, meshProcessInfo.indexCount);
    std::for_each(std::execution::seq, index_range.begin(), index_range.end(),
        [&](auto flatIndex) -> void {
            uint32_t faceIndex = flatIndex / faceIndexCount;
            uint32_t localIndex = flatIndex % faceIndexCount;

            indices[meshProcessInfo.indexOffset + flatIndex] = meshProcessInfo.vertexOffset + meshProcessInfo.mesh->mFaces[faceIndex].mIndices[localIndex];
        }
    );
}

void Model::ProcessMaterials(const aiScene* scene)
{
    Timer timer;

    auto material_index_range = std::views::iota(0u, (uint32_t)materials.size());
    std::for_each(std::execution::par, material_index_range.begin(), material_index_range.end(), 
        [&](auto materialIndex) -> void {
            ProcessMaterial(scene, materialIndex);
        }
    );

    std::cout << "-----------------------------------------\n";
    std::cout << "ProcessMaterials: " << timer.GetElapsedTime() << "\n";
    std::cout << "-----------------------------------------\n";
}

void Model::ProcessMaterial(const aiScene* scene, uint32_t materialIndex)
{
    aiMaterial* material = scene->mMaterials[materialIndex];
    MaterialComponent materialComponent;

    //Diffuse texture
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        std::string real_path = directory + "/" + std::string(path.C_Str());
        materialComponent.albedo = imageManager->LoadImage(real_path, true);
    }

    //Normals texture
    if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
        aiString path;
        material->GetTexture(aiTextureType_NORMALS, 0, &path);
        std::string real_path = directory + "/" + std::string(path.C_Str());
        materialComponent.normal = imageManager->LoadImage(real_path, true);
    }

    //Height texture
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0 && materialComponent.normal == nullptr)
    {
        aiString path;
        material->GetTexture(aiTextureType_HEIGHT, 0, &path);
        std::string real_path = directory + "/" + std::string(path.C_Str());
        materialComponent.normal = imageManager->LoadImage(real_path, true);
    }

    //Displacement
    if (material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0 && materialComponent.normal == nullptr)
    {
        aiString path;
        material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path);
        std::string real_path = directory + "/" + std::string(path.C_Str());
        materialComponent.normal = imageManager->LoadImage(real_path, true);
    }

    aiColor3D diffuseColor(1.f, 1.f, 1.f);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    materialComponent.color = glm::vec4(Assimp::ConvertAssimpToGlm(diffuseColor), 1);

    materials[materialIndex] = materialComponent;
}
