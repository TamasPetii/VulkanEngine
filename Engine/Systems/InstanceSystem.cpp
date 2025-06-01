#include "InstanceSystem.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Components/ModelComponent.h"
#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/PointLightComponent.h"

#include <future>
#include <thread>
#include <ranges>
#include <algorithm>

#include "Engine/Timer/Timer.h"

void InstanceSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex, float deltaTime)
{
	auto futureShape = std::async(std::launch::async, &InstanceSystem::UpdateShapeInstances, this, registry, resourceManager);
	auto futureModel = std::async(std::launch::async, &InstanceSystem::UpdateModelInstances, this, registry, resourceManager);
	auto futurePointLight = std::async(std::launch::async, &InstanceSystem::UpdatePointLightInstances, this, registry);

	futureShape.get();
	futureModel.get();
	futurePointLight.get();
}

void InstanceSystem::OnFinish(std::shared_ptr<Registry> registry)
{
}

void InstanceSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto futureShapeGpu = std::async(std::launch::async, &InstanceSystem::UpdateShapeInstancesGpu, this, resourceManager, frameIndex);
	auto futureModelGpu = std::async(std::launch::async, &InstanceSystem::UpdateModelInstancesGpu, this, resourceManager, frameIndex);
	auto futurePointLightGpu = std::async(std::launch::async, &InstanceSystem::UpdatePointLightInstancesGpu, this, registry, resourceManager, frameIndex);

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
		if (model && model->state == LoadState::Ready)
		{
			model->ResetInstanceCount();
			model->ReserveInstances(model.use_count());
		}
	}

	auto modelPool = registry->GetPool<ModelComponent>();
	if (!modelPool)
		return;

	std::for_each(std::execution::seq, modelPool->GetDenseIndices().begin(), modelPool->GetDenseIndices().end(),
		[&](Entity entity) -> void 	{
			auto& modelComponent = modelPool->GetData(entity);

			if (modelComponent.toRender && modelComponent.model && modelComponent.model->state == LoadState::Ready)
				modelComponent.model->AddIndex(modelPool->GetDenseIndex(entity));
		}
	);

	for (auto& [name, model] : modelManager->GetModels())
	{
		if (model && model->state == LoadState::Ready)
			model->ShrinkInstances();
	}
}

void InstanceSystem::UpdateModelInstancesGpu(std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto modelManager = resourceManager->GetModelManager();
	std::for_each(std::execution::par, modelManager->GetModels().begin(), modelManager->GetModels().end(),
		[&](const std::pair<std::string, std::shared_ptr<Model>>& data) -> void {
			if(data.second && data.second->state == LoadState::Ready)
				data.second->UploadInstanceDataToGPU(data.second.use_count(), frameIndex);
		}
	);
}

void InstanceSystem::UpdatePointLightInstances(std::shared_ptr<Registry> registry)
{
	auto pointLightPool = registry->GetPool<PointLightComponent>();
	if (!pointLightPool)
		return;

	PointLightComponent::instanceCount = 0;
	PointLightComponent::instanceIndices.clear();
	PointLightComponent::instanceIndices.reserve(pointLightPool->GetDenseSize());

	std::for_each(std::execution::seq, pointLightPool->GetDenseIndices().begin(), pointLightPool->GetDenseIndices().end(),
		[&](Entity entity) -> void {
			auto& pointLightComponent = pointLightPool->GetData(entity);

			if (pointLightComponent.toRender)
			{
				PointLightComponent::instanceIndices.push_back(pointLightPool->GetDenseIndex(entity));
				PointLightComponent::instanceCount++;
			}
		}
	);

	PointLightComponent::instanceIndices.resize(PointLightComponent::instanceCount);
}

void InstanceSystem::UpdatePointLightInstancesGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto pointLightPool = registry->GetPool<PointLightComponent>();
	if (!pointLightPool || PointLightComponent::instanceCount == 0)
		return;

	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * PointLightComponent::instanceCount;
		auto pointLightInstanceIndicesBufferHandler = resourceManager->GetComponentBufferManager()->GetComponentBuffer("PointLightInstanceIndices", frameIndex)->buffer->GetHandler();
		memcpy(pointLightInstanceIndicesBufferHandler, PointLightComponent::instanceIndices.data(), (size_t)bufferSize);
	}

	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * PointLightComponent::instanceCount;
		auto pointLightOcclusionIndicesBufferHandler = resourceManager->GetComponentBufferManager()->GetComponentBuffer("PointLightOcclusionIndices", frameIndex)->buffer->GetHandler();
		memset(pointLightOcclusionIndicesBufferHandler, 0, (size_t)bufferSize);
	}
}

void InstanceSystem::UpdatePointLightInstancesWithOcclusion(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto pointLightPool = registry->GetPool<PointLightComponent>();
	if (!pointLightPool || PointLightComponent::instanceCount == 0)
		return;

	auto pointLightOcclusionIndicesBufferHandler = static_cast<uint32_t*>(resourceManager->GetComponentBufferManager()->GetComponentBuffer("PointLightOcclusionIndices", frameIndex)->buffer->GetHandler());

	uint32_t currentIndex = 0;
	for (uint32_t i = 0; i < PointLightComponent::instanceCount; ++i)
	{
		if (pointLightOcclusionIndicesBufferHandler[i] == 1)
			PointLightComponent::instanceIndices[currentIndex++] = PointLightComponent::instanceIndices[i];
	}

	PointLightComponent::instanceCount = currentIndex;
	PointLightComponent::instanceIndices.resize(PointLightComponent::instanceCount);

	VkDeviceSize bufferSize = sizeof(uint32_t) * PointLightComponent::instanceCount;
	auto pointLightInstanceIndicesBufferHandler = resourceManager->GetComponentBufferManager()->GetComponentBuffer("PointLightInstanceIndices", frameIndex)->buffer->GetHandler();
	memcpy(pointLightInstanceIndicesBufferHandler, PointLightComponent::instanceIndices.data(), (size_t)bufferSize);
}