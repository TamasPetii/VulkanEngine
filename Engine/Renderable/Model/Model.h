#pragma once
#include "Engine/Renderable/Renderable.h"
#include "AssimpConverter.h"
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <assimp/types.h>
#include <Assimp/scene.h>
#include <filesystem>
#include <queue>

class ENGINE_API Model : public Renderable
{
public:
	bool Load(const std::string& path);
	uint32_t GetMeshCount();
	std::shared_ptr<Vk::Buffer> GetIndirectBuffer();
private:
	virtual void UploadToGpu();
	void PreFetch(aiNode* node, const aiScene* scene);
	void Process(aiNode* node, const aiScene* scene);
	void ProcessGeometry(aiMesh* mesh, const aiScene* scene, uint32_t& currentMeshCount, uint32_t& currentVertexCount, uint32_t& currentIndexCount);
private:
	uint32_t meshCount = 0;
	std::string path;
	std::string directory;
	std::shared_ptr<Vk::Buffer> materialBuffer;
	std::shared_ptr<Vk::Buffer> indirectBuffer;
	std::vector<VkDrawIndexedIndirectCommand> indirectCommands;
};

