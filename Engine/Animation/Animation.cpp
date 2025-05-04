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
    std::cout << std::format("[Thread Started] : Loading animation path={}", path) << "\n";

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


    PreFetch(scene);
}

void Animation::PreFetch(const aiScene* scene)
{
    /*
    std::queue<std::pair<aiNode*, uint32_t>> queue;
    queue.push({ scene->mRootNode, UINT32_MAX });

    while (!queue.empty())
    {
        auto [currentNode, parentBoneIndex] = queue.front();
        queue.pop();

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
                if (boneIndex.find(boneName) == boneIndex.end())
                {
                    boneIndex[boneName] = boneProcessInfos.size();

                    BoneProcessInfo boneProcessInfo{};
                    boneProcessInfo.name = boneName;
                    boneProcessInfo.offsetMatrix = Assimp::ConvertAssimpToGlm(bone->mOffsetMatrix);
                    boneProcessInfo.parentIndex = parentBoneIndex;
                    boneProcessInfos.push_back(boneProcessInfo);
                }
            }
            
            vertexCount += meshVertexCount;
            meshCount++;
        }

        for (unsigned int i = 0; i < currentNode->mNumChildren; ++i)
            queue.push(currentNode->mChildren[i]);
    }

    vertexBoneDatas.resize(vertexCount);
    */
}

void Animation::ProcessMeshVertexBones(const aiScene* scene)
{
    std::for_each(std::execution::seq, meshProcessInfos.begin(), meshProcessInfos.end(),
        [&](const auto& meshProcessInfo) -> void {
            ProcessMeshVertexBone(scene, meshProcessInfo);
        }
    );
}

void Animation::ProcessMeshVertexBone(const aiScene* scene, const MeshProcessInfo& meshProcessInfo)
{
    for (uint32_t b = 0; b < meshProcessInfo.mesh->mNumBones; ++b)
    {
        auto bone = meshProcessInfo.mesh->mBones[b];
        for (uint32_t w = 0; w < bone->mNumWeights; ++w)
        {
            for (uint32_t i = 0; i < 4; i++)
            {
                uint32_t vertexIndex = meshProcessInfo.vertexOffset + bone->mWeights[w].mVertexId;
                if (vertexBoneDatas[vertexIndex].indices[i] == -1)
                {
                    vertexBoneDatas[vertexIndex].indices[i] = boneIndex[std::string(bone->mName.C_Str())];
                    vertexBoneDatas[vertexIndex].weights[i] = bone->mWeights[w].mWeight;
                }
            }
        }
    }
}
