#include "MaterialSystem.h"

void MaterialSystem::OnStart(std::shared_ptr<Registry> registry)
{
}

void MaterialSystem::OnUpdate(std::shared_ptr<Registry> registry, float deltaTime)
{
	auto materialPool = registry->GetPool<MaterialComponent>();

	if (!materialPool)
		return;

	std::for_each(std::execution::par, materialPool->GetDenseEntities().begin(), materialPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void
		{
			if (materialPool->GetBitset(entity).test(UPDATE_BIT))
			{
				auto materialComponent = materialPool->GetComponent(entity);
				materialPool->GetBitset(entity).set(CHANGED_BIT, true);
				materialComponent->versionID++;
			}
		}
	);
}

void MaterialSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto materialPool = registry->GetPool<MaterialComponent>();

	if (!materialPool)
		return;

	std::for_each(std::execution::par, materialPool->GetDenseEntities().begin(), materialPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			materialPool->GetBitset(entity).set(REGENERATE_BIT, false);
			materialPool->GetBitset(entity).set(UPDATE_BIT, false);
			materialPool->GetBitset(entity).set(CHANGED_BIT, false);
		}
	);
}

void MaterialSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ComponentBufferManager> componentBufferManager, uint32_t frameIndex)
{
	auto materialPool = registry->GetPool<MaterialComponent>();

	if (!materialPool)
		return;

	auto componentBuffer = componentBufferManager->GetComponentBuffer("MaterialData", frameIndex);
	auto bufferHandler = static_cast<MaterialComponentGPU*>(componentBuffer->buffer->GetHandler());

	std::for_each(std::execution::par, materialPool->GetDenseEntities().begin(), materialPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			auto materialComponent = materialPool->GetComponent(entity);
			auto index = materialPool->GetIndex(entity);

			if (componentBuffer->versions[index] != materialComponent->versionID)
			{
				componentBuffer->versions[index] = materialComponent->versionID;
				bufferHandler[index] = MaterialComponentGPU(*materialComponent);
			}
		}
	);
}
