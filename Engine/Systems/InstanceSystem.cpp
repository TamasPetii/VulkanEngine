#include "InstanceSystem.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Components/ModelComponent.h"
#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/TransformComponent.h"

#include <future>
#include <thread>

#include "Engine/Timer/Timer.h"

void InstanceSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto futureShape = std::async(std::launch::async, &InstanceSystem::UpdateShapeInstances, this, registry, resourceManager);
	auto futureModel = std::async(std::launch::async, &InstanceSystem::UpdateModelInstances, this, registry, resourceManager);

	futureShape.get();
	futureModel.get();
}

void InstanceSystem::OnFinish(std::shared_ptr<Registry> registry)
{
}

void InstanceSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto futureShapeGpu = std::async(std::launch::async, &InstanceSystem::UpdateShapeInstancesGpu, this, resourceManager, frameIndex);
	auto futureModelGpu = std::async(std::launch::async, &InstanceSystem::UpdateModelInstancesGpu, this, resourceManager, frameIndex);

	futureShapeGpu.get();
	futureModelGpu.get();
}

void InstanceSystem::UpdateShapeInstances(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager)
{
	auto geometryManager = resourceManager->GetGeometryManager();
	for (auto& [name, shape] : geometryManager->GetShapes())
	{
		shape->ResetInstanceCount();
		shape->ReserveInstances(shape.use_count());
	}

	auto shapePool = registry->GetPool<ShapeComponent>();
	if (!shapePool)
		return;

	std::for_each(std::execution::seq, shapePool->GetDenseIndices().begin(), shapePool->GetDenseIndices().end(),
		[&](Entity entity) -> void {
			auto& shapeComponent = shapePool->GetData(entity);

			if (shapeComponent.toRender && shapeComponent.shape)
				shapeComponent.shape->AddIndex(shapePool->GetDenseIndex(entity));
		}
	);

	for (auto& [name, shape] : geometryManager->GetShapes())
		shape->ShrinkInstances();
}

void InstanceSystem::UpdateShapeInstancesGpu(std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto geometryManager = resourceManager->GetGeometryManager();
	std::for_each(std::execution::par, geometryManager->GetShapes().begin(), geometryManager->GetShapes().end(),
		[&](const std::pair<std::string, std::shared_ptr<Shape>>& data) -> void {
			data.second->UploadInstanceDataToGPU(data.second.use_count(), frameIndex);
		}
	);
}

void InstanceSystem::UpdateModelInstances(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager)
{
	auto modelManager = resourceManager->GetModelManager();
	for (auto& [name, model] : modelManager->GetModels())
	{
		model->ResetInstanceCount();
		model->ReserveInstances(model.use_count());
	}

	auto modelPool = registry->GetPool<ModelComponent>();
	if (!modelPool)
		return;

	std::for_each(std::execution::seq, modelPool->GetDenseIndices().begin(), modelPool->GetDenseIndices().end(),
		[&](Entity entity) -> void 	{
			auto& modelComponent = modelPool->GetData(entity);

			if (modelComponent.toRender && modelComponent.model)
				modelComponent.model->AddIndex(modelPool->GetDenseIndex(entity));
		}
	);

	for (auto& [name, model] : modelManager->GetModels())
		model->ShrinkInstances();
}

void InstanceSystem::UpdateModelInstancesGpu(std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto modelManager = resourceManager->GetModelManager();
	std::for_each(std::execution::par, modelManager->GetModels().begin(), modelManager->GetModels().end(),
		[&](const std::pair<std::string, std::shared_ptr<Model>>& data) -> void {
			data.second->UploadInstanceDataToGPU(data.second.use_count(), frameIndex);
		}
	);
}
