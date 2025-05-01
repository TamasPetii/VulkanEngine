#include "MaterialSystem.h"

void MaterialSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto materialPool = registry->GetPool<MaterialComponent>();

	if (!materialPool)
		return;

	std::for_each(std::execution::par, materialPool->GetDenseIndices().begin(), materialPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void
		{
			[[unlikely]]
			if (materialPool->IsBitSet<UPDATE_BIT>(entity))
			{
				auto& materialComponent = materialPool->GetData(entity);
				materialPool->SetBit<CHANGED_BIT>(entity);
				materialComponent.versionID++;
			}
		}
	);
}

void MaterialSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto materialPool = registry->GetPool<MaterialComponent>();

	if (!materialPool)
		return;

	std::for_each(std::execution::par, materialPool->GetDenseIndices().begin(), materialPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			[[unlikely]]
			if(materialPool->IsBitSet<CHANGED_BIT>(entity))
				materialPool->GetBitset(entity).reset();
		}
	);
}

void MaterialSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto materialPool = registry->GetPool<MaterialComponent>();

	if (!materialPool)
		return;

	auto componentBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("MaterialData", frameIndex);
	auto bufferHandler = static_cast<MaterialComponentGPU*>(componentBuffer->buffer->GetHandler());

	std::for_each(std::execution::par, materialPool->GetDenseIndices().begin(), materialPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			auto& materialComponent = materialPool->GetData(entity);
			auto materialIndex = materialPool->GetDenseIndex(entity);

			[[unlikely]]
			if (componentBuffer->versions[materialIndex] != materialComponent.versionID)
			{
				componentBuffer->versions[materialIndex] = materialComponent.versionID;
				bufferHandler[materialIndex] = MaterialComponentGPU(materialComponent);
			}
		}
	);
}
