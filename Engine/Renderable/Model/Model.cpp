#include "Model.h"

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

std::shared_ptr<Vk::Buffer> Model::GetIndirectBuffer()
{
    return indirectBuffer;
}

void Model::UploadToGpu()
{
    Renderable::UploadToGpu();

    auto device = Vk::VulkanContext::GetContext()->GetDevice();

    //Upload the indirect commands to the gpu
    {
        VkDeviceSize bufferSize = sizeof(VkDrawIndexedIndirectCommand) * indirectCommands.size();

        Vk::BufferConfig config;
        config.size = bufferSize;
        config.usage = VK_BUFFER_USAGE_2_INDIRECT_BUFFER_BIT;
        config.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        indirectBuffer = std::make_shared<Vk::Buffer>(config);
        indirectBuffer->MapMemory();

        memcpy(indirectBuffer->GetHandler(), indirectCommands.data(), (size_t)bufferSize);

        indirectCommands.clear();
        indirectCommands.shrink_to_fit();
    }
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
    indirectCommands.resize(meshCount);
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
}

void Model::ProcessGeometry(aiMesh* mesh, const aiScene* scene, uint32_t& currentMeshCount, uint32_t& currentVertexCount, uint32_t& currentIndexCount)
{
    uint32_t meshVerticesCount = 0;

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

        vertices.emplace_back(Vertex{position, normal, texcoord});
        meshVerticesCount++;
    }

    uint32_t meshIndicesCount = 0;

    //Index Data
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
            meshIndicesCount++;
        }
    }
    
    indirectCommands[currentMeshCount].indexCount = meshIndicesCount;
    indirectCommands[currentMeshCount].firstIndex = currentIndexCount;
    indirectCommands[currentMeshCount].vertexOffset = currentVertexCount;
    indirectCommands[currentMeshCount].instanceCount = 1;
    indirectCommands[currentMeshCount].firstInstance = 0;

    currentIndexCount += meshIndicesCount;
    currentVertexCount += meshVerticesCount;
    currentMeshCount++;
}
