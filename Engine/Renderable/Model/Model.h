#pragma once
#include "Engine/Renderable/Renderable.h"
#include "Engine/Renderable/Materialized.h"
#include "Engine/Renderable/Instanceable.h"
#include "Engine/Renderable/BoundingVolume.h"

#include "AssimpConverter.h"
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <assimp/types.h>
#include <Assimp/scene.h>
#include <filesystem>
#include <queue>

#include "Engine/Managers/ImageManager.h"

struct ENGINE_API MeshProcessInfo
{
	aiMesh* mesh;
	uint32_t meshIndex;
	uint32_t vertexOffset;
	uint32_t vertexCount;
	uint32_t indexOffset;
	uint32_t indexCount;
	glm::mat4 transform;
};

class ENGINE_API Model : public Renderable, public Materialized, public Instanceable, public BoundingVolume
{
public:
	Model(std::shared_ptr<ImageManager> imageManager);
	bool Load(const std::string& path);
	uint32_t GetMeshCount();
private:
	virtual void UploadToGpu();
	virtual void PopulateSurfacePoints() override;
	void PreFetch(aiNode* node, const aiScene* scene);
	void Process(aiNode* node, const aiScene* scene);

	void ProcessMeshVertices(const aiScene* scene);
	void ProcessMeshVertex(const aiScene* scene, const MeshProcessInfo& meshProcessInfo);

	void ProcessMeshIndices(const aiScene* scene);
	void ProcessMeshIndex(const aiScene* scene, const MeshProcessInfo& meshProcessInfo);

	void ProcessMaterials(const aiScene* scene);
	void ProcessMaterial(const aiScene* scene, uint32_t materialIndex);
private:
	std::string path;
	std::string directory;
private:
	uint32_t meshCount = 0;
	std::vector<MeshProcessInfo> meshProcessInfos;
private:
	std::shared_ptr<ImageManager> imageManager;
};

