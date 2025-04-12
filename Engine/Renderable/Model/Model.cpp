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

void Model::PreFetch(aiNode* node, const aiScene* scene)
{
    std::queue<aiNode*> queue;
    queue.push(node);

    while (!queue.empty())
    {
        aiNode* currentNode = queue.front();
        queue.pop();

        for (unsigned int i = 0; i < currentNode->mNumMeshes; ++i)
        {
            aiMesh* currentMesh = scene->mMeshes[currentNode->mMeshes[i]];
            vertexCount += currentMesh->mNumVertices;

            for (unsigned int f = 0; f < currentMesh->mNumFaces; ++f)
                indexCount += currentMesh->mFaces[f].mNumIndices;

            meshCount++;
        }

        for (unsigned int i = 0; i < currentNode->mNumChildren; ++i)
            queue.push(currentNode->mChildren[i]);
    }

    vertices.reserve(vertexCount);
    indices.reserve(indexCount);
    surfacePoints.reserve(vertexCount);
    materialIndices.reserve(meshCount);
}

void Model::Process(aiNode* node, const aiScene* scene)
{
    std::queue<aiNode*> queue;
    queue.push(node);

    uint32_t currentMeshCount = 0;
    uint32_t currentVertexCount = 0;
    uint32_t currentIndexCount = 0;

    while (!queue.empty())
    {
        aiNode* currentNode = queue.front();
        queue.pop();

        for (uint32_t i = 0; i < currentNode->mNumMeshes; ++i)
        {
            aiMesh* currentMesh = scene->mMeshes[currentNode->mMeshes[i]];
            ProcessGeometry(currentMesh, scene, currentMeshCount, currentVertexCount, currentIndexCount);
        }

        for (uint32_t i = 0; i < currentNode->mNumChildren; ++i)
        {
            queue.push(currentNode->mChildren[i]);
        }
    }

    for (auto& vertex : vertices)
        surfacePoints.push_back(vertex.position);
}

void Model::ProcessGeometry(aiMesh* mesh, const aiScene* scene, uint32_t& currentMeshCount, uint32_t& currentVertexCount, uint32_t& currentIndexCount)
{
    uint32_t meshVerticesCount = 0;

    std::string materialName = "";
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
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
                materialComponent.albedo = imageManager->LoadImage(real_path);
            }

            //Normals texture
            if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
            {
                aiString path;
                material->GetTexture(aiTextureType_NORMALS, 0, &path);
                std::string real_path = directory + "/" + std::string(path.C_Str());
                materialComponent.normal = imageManager->LoadImage(real_path);
            }

            //Height texture
            if (material->GetTextureCount(aiTextureType_HEIGHT) > 0 && materialComponent.normal == nullptr)
            {
                aiString path;
                material->GetTexture(aiTextureType_HEIGHT, 0, &path);
                std::string real_path = directory + "/" + std::string(path.C_Str());
                materialComponent.normal = imageManager->LoadImage(real_path);
            }

            //Displacement
            if (material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0 && materialComponent.normal == nullptr)
            {
                aiString path;
                material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path);
                std::string real_path = directory + "/" + std::string(path.C_Str());
                materialComponent.normal = imageManager->LoadImage(real_path);
            }

            aiColor3D diffuseColor(1.f, 1.f, 1.f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            materialComponent.color = glm::vec4(Assimp::ConvertAssimpToGlm(diffuseColor), 1);

            loadedMaterials[materialName] = materials.size();
            materials.push_back(materialComponent);
        }
    }

    //Vertex Data Process
    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
        //Position
        glm::vec3 position{ 0,0,0 };
        if (mesh->mVertices)
            position = Assimp::ConvertAssimpToGlm(mesh->mVertices[i]);

        //Normals
        glm::vec3 normal{ 0,0,0 };
        if (mesh->mNormals)
            normal = Assimp::ConvertAssimpToGlm(mesh->mNormals[i]);

        //Tangents
        glm::vec3 tangent{ 0,0,0 };
        if (mesh->mTangents)
            tangent = Assimp::ConvertAssimpToGlm(mesh->mTangents[i]);

        //Textures
        glm::vec2 texcoord{ 0,0 };
        if (mesh->mTextureCoords[0] != nullptr)
        {
            texcoord.x = mesh->mTextureCoords[0][i].x;
            texcoord.y = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(Vertex(position, normal, tangent, texcoord, loadedMaterials[materialName]));
        meshVerticesCount++;
    }

    uint32_t meshIndicesCount = 0;

    //Index Data
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(currentVertexCount + face.mIndices[j]);
            meshIndicesCount++;
        }
    }

    currentIndexCount += meshIndicesCount;
    currentVertexCount += meshVerticesCount;
    currentMeshCount++;
}
