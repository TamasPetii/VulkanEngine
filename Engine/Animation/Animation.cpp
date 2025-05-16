#include "Animation.h"
#include "Engine/Timer/Timer.h"
#include "Engine/Renderable/Model/AssimpConverter.h"

#include <algorithm>
#include <execution>
#include <iostream>
#include <format>
#include <queue>
#include <ranges>

void Animation::Load(const std::string& path)
{
    Timer timer;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        state = LoadState::Failed;
        return;
    }

    if (!scene->HasAnimations())
    {
        state = LoadState::Failed;
        return;
    }

    duration = scene->mAnimations[0]->mDuration;
    ticksPerSecond = scene->mAnimations[0]->mTicksPerSecond;

    PreFetch(scene);
    ProcessMeshVertexBones();
    ProcessBoneKeyFrames(scene);

    state = LoadState::CpuLoaded;

    InitVertexBoneBuffer();

    state = LoadState::GpuUploaded;
}

void Animation::PreFetch(const aiScene* scene)
{
    std::queue<std::pair<aiNode*, uint32_t>> queue;
    queue.push({ scene->mRootNode, UINT32_MAX });

    while (!queue.empty())
    {
        auto [currentNode, parentNodeIndex] = queue.front();
        queue.pop();

        uint32_t nodeIndex = nodeProcessInfos.size();

        NodeProcessInfo nodeProcessInfo;
        nodeProcessInfo.name = std::string(currentNode->mName.C_Str());
        nodeProcessInfo.parentIndex = parentNodeIndex;
        nodeIndexMap[nodeProcessInfo.name] = nodeProcessInfos.size();
        nodeProcessInfos.push_back(nodeProcessInfo);

        for (uint32_t i = 0; i < currentNode->mNumMeshes; ++i)
        {
            aiMesh* currentMesh = scene->mMeshes[currentNode->mMeshes[i]];

            uint32_t meshVertexOffset = vertexCount;
            uint32_t meshVertexCount = currentMesh->mNumVertices;

            MeshProcessInfo meshProcessInfo{};
            meshProcessInfo.mesh = currentMesh;
            meshProcessInfo.meshIndex = meshCount;
            meshProcessInfo.vertexOffset = meshVertexOffset;
            meshProcessInfo.vertexCount = meshVertexCount;
            meshProcessInfos.push_back(meshProcessInfo);

            for (uint32_t b = 0; b < currentMesh->mNumBones; ++b)
            {
                auto bone = currentMesh->mBones[b];
                std::string boneName = std::string(bone->mName.C_Str());
                if (boneIndexMap.find(boneName) == boneIndexMap.end())
                {
                    boneIndexMap[boneName] = boneCount++;

                    BoneProcessInfo boneProcessInfo{};
                    boneProcessInfo.name = boneName;
                    boneProcessInfo.offsetMatrix = Assimp::ConvertAssimpToGlm(bone->mOffsetMatrix);
                    boneProcessInfos.push_back(boneProcessInfo);
                }
            }
            
            vertexCount += meshVertexCount;
            meshCount++;
        }

        for (unsigned int i = 0; i < currentNode->mNumChildren; ++i)
            queue.push({ currentNode->mChildren[i], nodeIndex });
    }

    for (auto& nodeProcessInfo : nodeProcessInfos)
    {
        if (boneIndexMap.find(nodeProcessInfo.name) != boneIndexMap.end())
            nodeProcessInfo.boneIndex = boneIndexMap[nodeProcessInfo.name];
    }

    vertexBoneDatas.resize(vertexCount);
}

void Animation::ProcessMeshVertexBones()
{
    std::for_each(std::execution::seq, meshProcessInfos.begin(), meshProcessInfos.end(),
        [&](const auto& meshProcessInfo) -> void {
            ProcessMeshVertexBone(meshProcessInfo);
        }
    );
}

void Animation::ProcessMeshVertexBone(const MeshProcessInfo& meshProcessInfo)
{
    for (uint32_t b = 0; b < meshProcessInfo.mesh->mNumBones; ++b)
    {
        auto bone = meshProcessInfo.mesh->mBones[b];
        uint32_t nodeIndex = nodeIndexMap.at(std::string(bone->mName.C_Str()));
        for (uint32_t w = 0; w < bone->mNumWeights; ++w)
        {
            uint32_t vertexIndex = meshProcessInfo.vertexOffset + bone->mWeights[w].mVertexId;      
            for (uint32_t i = 0; i < 4; i++)
            {
                if (vertexBoneDatas[vertexIndex].indices[i] == UINT32_MAX)
                {
                    vertexBoneDatas[vertexIndex].indices[i] = nodeIndex;
                    vertexBoneDatas[vertexIndex].weights[i] = bone->mWeights[w].mWeight;
                    break;
                }
            }
        }
    }
}

void Animation::ProcessBoneKeyFrames(const aiScene* scene)
{
    auto animation = scene->mAnimations[0];
    auto index_range = std::views::iota(0u, animation->mNumChannels);
    std::for_each(std::execution::seq, index_range.begin(), index_range.end(),
        [&](auto channelIndex) -> void {
            auto channel = animation->mChannels[channelIndex];

            std::string channelName = std::string(channel->mNodeName.C_Str());
            
            if (boneIndexMap.find(channelName) != boneIndexMap.end())
            {
                boneProcessInfos[boneIndexMap.at(channelName)].bone = Bone(channel);
            }
            else if(nodeIndexMap.find(channelName) != nodeIndexMap.end())
            {            
                boneIndexMap[channelName] = boneCount++;
                BoneProcessInfo boneProcessInfo;
                boneProcessInfo.bone = Bone(channel);
                boneProcessInfo.name = channelName;
                boneProcessInfo.offsetMatrix = glm::mat4(1);
                boneProcessInfos.push_back(boneProcessInfo);
                nodeProcessInfos[nodeIndexMap[channelName]].boneIndex = boneIndexMap.at(channelName);
            }
        }
    );
}

void Animation::InitVertexBoneBuffer()
{
    VkDeviceSize vertexBoneBufferSize = sizeof(VertexBoneData) * vertexBoneDatas.size();

    Vk::BufferConfig vertexBoneStagingConfig;
    vertexBoneStagingConfig.size = vertexBoneBufferSize;
    vertexBoneStagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
    vertexBoneStagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    Vk::Buffer vertexBoneStagingBuffer{ vertexBoneStagingConfig };

    void* vertexBoneStagigngBufferHandler = vertexBoneStagingBuffer.MapMemory();
    memcpy(vertexBoneStagigngBufferHandler, vertexBoneDatas.data(), vertexBoneBufferSize);
    vertexBoneStagingBuffer.UnmapMemory();

    Vk::BufferConfig vertexBoneBufferConfig;
    vertexBoneBufferConfig.size = vertexBoneBufferSize;
    vertexBoneBufferConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
    vertexBoneBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vertexBoneBuffer = std::make_shared<Vk::Buffer>(vertexBoneBufferConfig);

    Vk::VulkanContext::GetContext()->GetImmediateQueue()->SubmitTransfer(
        [&](VkCommandBuffer commandBuffer) -> void {
            Vk::Buffer::CopyBufferToBuffer(commandBuffer, vertexBoneStagingBuffer.Value(), vertexBoneBuffer->Value(), vertexBoneBufferSize);
        }
    );

    vertexBoneDatas.clear();
}

void Animation::InitBoundingVolumeBuffer()
{
    /*
    VkDeviceSize aabbBufferConfig = sizeof(AABBGPU) * vertexBoneDatas.size();

    Vk::BufferConfig vertexBoneStagingConfig;
    vertexBoneStagingConfig.size = vertexBoneBufferSize;
    vertexBoneStagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
    vertexBoneStagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    Vk::Buffer vertexBoneStagingBuffer{ vertexBoneStagingConfig };

    void* vertexBoneStagigngBufferHandler = vertexBoneStagingBuffer.MapMemory();
    memcpy(vertexBoneStagigngBufferHandler, vertexBoneDatas.data(), vertexBoneBufferSize);
    vertexBoneStagingBuffer.UnmapMemory();

    Vk::BufferConfig aabbBufferConfig;
    aabbBufferConfig.size = vertexBoneBufferSize;
    aabbBufferConfig.usage = VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
    aabbBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    vertexBoneBuffer = std::make_shared<Vk::Buffer>(vertexBoneBufferConfig);
    */
}
