#pragma once
#include "Engine/Renderable/Renderable.h"
#include "Engine/Renderable/IndirectMultiDrawable.h"
#include "Engine/Renderable/Materialized.h"
#include "Engine/Renderable/Instanceable.h"

#include "AssimpConverter.h"
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <assimp/types.h>
#include <Assimp/scene.h>
#include <filesystem>
#include <queue>

#include "Engine/Managers/ImageManager.h"

class ENGINE_API Model : public Renderable, public Materialized, public Instanceable
{
public:
	Model(std::shared_ptr<ImageManager> imageManager);
	bool Load(const std::string& path);
	uint32_t GetMeshCount();
private:
	virtual void UploadToGpu();
	void PreFetch(aiNode* node, const aiScene* scene);
	void Process(aiNode* node, const aiScene* scene);
	void ProcessGeometry(aiMesh* mesh, const aiScene* scene, uint32_t& currentMeshCount, uint32_t& currentVertexCount, uint32_t& currentIndexCount);
private:
	uint32_t meshCount = 0;
	std::string path;
	std::string directory;
	std::unordered_map<std::string, uint32_t> loadedMaterials;
private:
	std::shared_ptr<ImageManager> imageManager;
};

