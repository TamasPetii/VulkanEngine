#include "AnimationSystem.h"

#include "Engine/Components/ModelComponent.h"
#include "Engine/Components/AnimationComponent.h"

void AnimationSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex, float deltaTime)
{
	auto [animationPool, modelPool] = registry->GetPools<AnimationComponent, ModelComponent>();
	if (!animationPool || !modelPool)
		return;

	std::for_each(std::execution::par, animationPool->GetDenseIndices().begin(), animationPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			auto& animationComponent = animationPool->GetData(entity);

			if (animationComponent.animation && animationComponent.animation->state == LoadState::Ready)
			{
				[[unlikely]]
				if (animationPool->IsBitSet<REGENERATE_BIT>(entity))
				{
					//Model handles component indices -> need to update animation index to gpu.
					if (modelPool && modelPool->HasComponent(entity))
						modelPool->SetBit<UPDATE_BIT>(entity);

					animationComponent.nodeTransformVersion++;
					animationPool->ResetBit<REGENERATE_BIT>(entity);
				}

				[[unlikely]]
				if (animationComponent.nodeTransformBuffers[frameIndex].version != animationComponent.nodeTransformVersion)
				{
					animationComponent.nodeTransformBuffers[frameIndex].version = animationComponent.nodeTransformVersion;

					uint32_t nodeCount = animationPool->GetData(entity).animation->GetNodeProcessInfo().size();

					animationComponent.animationNodeTransforms.clear();
					animationComponent.animationNodeTransforms.resize(nodeCount);

					VkDeviceSize nodeBufferSize = sizeof(NodeTransform) * nodeCount;

					Vk::BufferConfig nodeBufferConfig;
					nodeBufferConfig.size = nodeBufferSize;
					nodeBufferConfig.usage = VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
					nodeBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

					animationComponent.nodeTransformBuffers[frameIndex].buffer = std::make_shared<Vk::Buffer>(nodeBufferConfig);
					animationComponent.nodeTransformBuffers[frameIndex].buffer->MapMemory();
				}
			}
		}
	);

	std::for_each(std::execution::par, animationPool->GetDenseIndices().begin(), animationPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			if (animationPool->GetData(entity).animation &&
				animationPool->GetData(entity).animation->state == LoadState::Ready &&
				modelPool->HasComponent(entity) && 	
				modelPool->GetData(entity).model && 
				modelPool->GetData(entity).model->state == LoadState::Ready &&
				(true || animationPool->IsBitSet<UPDATE_BIT>(entity)))
			{
				auto& animationComponent = animationPool->GetData(entity);
				auto& modelComponent = modelPool->GetData(entity);
				animationComponent.time += animationComponent.animation->GetTicksPerSeconds() * deltaTime * animationComponent.speed;
				animationComponent.time = fmod(animationComponent.time, animationComponent.animation->GetDuration());

				auto& boneProcesInfos = animationComponent.animation->GetBoneProcessInfo();
				auto& nodeProcessInfos = animationComponent.animation->GetNodeProcessInfo();
				auto& nodeTransformInfos = modelComponent.model->GetNodeTransformInfos();
				for (uint32_t i = 0; i < nodeProcessInfos.size(); ++i)
				{
					animationComponent.animationNodeTransforms[i].localTransform = nodeTransformInfos[i].localTransform;

					if (nodeProcessInfos[i].boneIndex != UINT32_MAX)
					{
						auto& boneProcessInfo = boneProcesInfos[nodeProcessInfos[i].boneIndex];
						animationComponent.animationNodeTransforms[i].localTransform = boneProcessInfo.bone.GetTransform(animationComponent.time);
						animationComponent.animationNodeTransforms[i].nodeTransform.transform = animationComponent.animationNodeTransforms[i].localTransform * boneProcessInfo.offsetMatrix;
					}

					if (nodeProcessInfos[i].parentIndex != UINT32_MAX)
					{
						auto& parentTransform = animationComponent.animationNodeTransforms[nodeProcessInfos[i].parentIndex].localTransform;
						animationComponent.animationNodeTransforms[i].localTransform = parentTransform * animationComponent.animationNodeTransforms[i].localTransform;
						animationComponent.animationNodeTransforms[i].nodeTransform.transform = parentTransform * animationComponent.animationNodeTransforms[i].nodeTransform.transform;
					}
					
					animationComponent.animationNodeTransforms[i].nodeTransform.transformIT = glm::inverse(glm::transpose(animationComponent.animationNodeTransforms[i].nodeTransform.transform));
				}

				animationPool->ResetBit<UPDATE_BIT>(entity);
				animationPool->SetBit<CHANGED_BIT>(entity);
			}
		}
	);
}

void AnimationSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto animationPool = registry->GetPool<AnimationComponent>();

	if (!animationPool)
		return;

	std::for_each(std::execution::par, animationPool->GetDenseIndices().begin(), animationPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			[[unlikely]]
			if (animationPool->IsBitSet<CHANGED_BIT>(entity))
				animationPool->GetBitset(entity).reset();
		}
	);
}

void AnimationSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto [animationPool, modelPool] = registry->GetPools<AnimationComponent, ModelComponent>();

	if (!animationPool || !modelPool)
		return;

	//TODO: UPDATE ANIMATION TRANSFORM BUFFER DEVICE ADDRESS

	std::for_each(std::execution::par, animationPool->GetDenseIndices().begin(), animationPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			if (animationPool->GetData(entity).animation &&
				animationPool->GetData(entity).animation->state == LoadState::Ready &&
				modelPool->HasComponent(entity) &&
				modelPool->GetData(entity).model &&
				modelPool->GetData(entity).model->state == LoadState::Ready)
			{
				auto& animationComponent = animationPool->GetData(entity);
				auto animationIndex = animationPool->GetDenseIndex(entity);		

				NodeTransform* animationBufferHandler = static_cast<NodeTransform*>(animationComponent.nodeTransformBuffers[frameIndex].buffer->GetHandler());

				for (uint32_t i = 0; i < animationComponent.animationNodeTransforms.size(); ++i)
					animationBufferHandler[i] = animationComponent.animationNodeTransforms[i].nodeTransform;
			}
		}
	);
}
