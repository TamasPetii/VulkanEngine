#pragma once
#include "Engine/EngineApi.h"
#include "Engine/Async/AsyncLoaded.h"
#include "Engine/Vulkan/Buffer.h"
#include "Bone.h"

#include <glm/glm.hpp>
#include <string>

#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <assimp/types.h>
#include <Assimp/scene.h>

class ENGINE_API Animation : public AsyncLoaded
{
private:
	struct ENGINE_API VertexBoneData
	{
		glm::vec4 weights = { 0.f, 0.f, 0.f, 0.f };
		glm::ivec4 indices = { -1, -1, -1, -1 };
	};
	struct ENGINE_API MeshProcessInfo
	{
		aiMesh* mesh;
		uint32_t meshIndex;
		uint32_t vertexOffset;
		uint32_t vertexCount;
	};
	struct ENGINE_API BoneProcessInfo
	{
		Bone bone;
		std::string name;
		glm::mat4 offsetMatrix;
	};
	struct ENGINE_API NodeProcessInfo
	{
		std::string name;
		uint32_t boneIndex = UINT32_MAX;
		uint32_t parentIndex = UINT32_MAX;
	};
	struct ENGINE_API NodeTransformGLSL
	{
		glm::mat4 transform;
		glm::mat4 transformIT;
	};
public:
	void Load(const std::string& path);
private:
	void PreFetch(const aiScene* scene);
	void ProcessBoneKeyFrames(const aiScene* scene);
	void ProcessMeshVertexBones();
	void ProcessMeshVertexBone(const MeshProcessInfo& meshProcessInfo);
	void InitVertexBoneBuffer();
	void InitNodeTransformBuffer();

	double duration;
	double ticksPerSecond;

	uint32_t meshCount = 0;
	uint32_t boneCount = 0;
	uint32_t vertexCount = 0;
	std::vector<VertexBoneData> vertexBoneDatas;
	std::shared_ptr<Vk::Buffer> vertexBoneBuffer;
	std::vector<NodeProcessInfo> nodeProcessInfos;
	std::vector<MeshProcessInfo> meshProcessInfos;
	std::vector<BoneProcessInfo> boneProcessInfos;
	std::unordered_map<std::string, uint32_t> boneIndex;
};

