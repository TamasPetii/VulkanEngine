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

	//UpdateShapeInstances(registry, resourceManager);
	//UpdateModelInstances(registry, resourceManager);
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

	//UpdateShapeInstancesGpu(resourceManager, frameIndex);
	//UpdateModelInstancesGpu(resourceManager, frameIndex);
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

	std::for_each(std::execution::seq, shapePool->GetDenseEntities().begin(), shapePool->GetDenseEntities().end(),
		[&](Entity entity) -> void {
			auto shapeComponent = shapePool->GetComponent(entity);

			if (shapeComponent->toRender && shapeComponent->shape)
				shapeComponent->shape->AddIndex(shapePool->GetIndex(entity));
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
			data.second->UploadInstanceDataToGPU(frameIndex);
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

	std::for_each(std::execution::seq, modelPool->GetDenseEntities().begin(), modelPool->GetDenseEntities().end(), 
		[&](Entity entity) -> void 	{
			auto modelComponent = modelPool->GetComponent(entity);

			if (modelComponent->toRender && modelComponent->model)
				modelComponent->model->AddIndex(modelPool->GetIndex(entity));
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
			data.second->UploadInstanceDataToGPU(frameIndex);
		}
	);
}
