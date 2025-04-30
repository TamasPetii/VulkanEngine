#include "Model.h"

Model::Model(std::shared_ptr<ImageManager> imageManager) : 
    imageManager(imageManager)
{
}

bool Model::Load(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        return false;

    this->path = path;
    this->directory = path.substr(0, path.find_last_of('/'));

    PreFetch(scene->mRootNode, scene);
    Process(scene->mRootNode, scene);
    //PopulateSurfacePoints();
    GenerateBoundingVolume(aabbMin, aabbMax);
    UploadToGpu();
    return true;
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

    aabbMax = glm::vec3(std::numeric_limits<float>::lowest());
    aabbMin = glm::vec3(std::numeric_limits<float>::max());
}

void Model::Process(aiNode* node, const aiScene* scene)
{
    for(uint32_t i = 0; i < meshProcessInfos.size(); ++i)
        ProcessGeometry(scene, i);
}

void Model::ProcessGeometry(const aiScene* scene, uint32_t meshIndex)
{
    auto& meshProcessInfo = meshProcessInfos[meshIndex];
    glm::mat4 transformIT = glm::inverse(glm::transpose(meshProcessInfo.transform));

    std::string materialName = "";
    if (meshProcessInfo.mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[meshProcessInfo.mesh->mMaterialIndex];
        materialName = std::string(material->GetName().C_Str());

        if (loadedMaterials.find(materialName) == loadedMaterials.end())
        {
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

            loadedMaterials[materialName] = materials.size();
            materials.push_back(materialComponent);
        }
    }

    for (uint32_t i = 0; i < meshProcessInfo.vertexCount; ++i)
    {
        //Position
        glm::vec3 position{ 0,0,0 };
        if (meshProcessInfo.mesh->mVertices)
        {
            position = Assimp::ConvertAssimpToGlm(meshProcessInfo.mesh->mVertices[i]);
            position = glm::vec3(meshProcessInfo.transform * glm::vec4(position, 1.0f));
            aabbMax = glm::max(aabbMax, position);
            aabbMin = glm::min(aabbMin, position);
        }

        //Normals
        glm::vec3 normal{ 0,0,0 };
        if (meshProcessInfo.mesh->mNormals)
        {
            normal = Assimp::ConvertAssimpToGlm(meshProcessInfo.mesh->mNormals[i]);
            normal = glm::vec3(transformIT * glm::vec4(normal, 0));
        }

        //Tangents
        glm::vec3 tangent{ 0,0,0 };
        if (meshProcessInfo.mesh->mTangents)
        {
            tangent = Assimp::ConvertAssimpToGlm(meshProcessInfo.mesh->mTangents[i]);
            tangent = glm::vec3(transformIT * glm::vec4(tangent, 0));
        }

        //Textures
        glm::vec2 texcoord{ 0,0 };
        if (meshProcessInfo.mesh->mTextureCoords[0] != nullptr)
        {
            texcoord.x = meshProcessInfo.mesh->mTextureCoords[0][i].x;
            texcoord.y = meshProcessInfo.mesh->mTextureCoords[0][i].y;
        }

        vertices[meshProcessInfo.vertexOffset + i] = Vertex(position, normal, tangent, texcoord, loadedMaterials[materialName]);
    }

    constexpr uint32_t faceIndexCount = 3;
    for (uint32_t i = 0; i < meshProcessInfo.indexCount / faceIndexCount; ++i)
    {
        for (uint32_t j = 0; j < faceIndexCount; ++j)
        {
            indices[meshProcessInfo.indexOffset + i * faceIndexCount + j] = meshProcessInfo.vertexOffset + meshProcessInfo.mesh->mFaces[i].mIndices[j];
        }
    }
}
