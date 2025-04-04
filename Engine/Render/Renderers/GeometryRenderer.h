#pragma once
#include "Engine/Render/RenderContext.h"
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Vulkan/Buffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ComponetBufferManager.h"
#include "Engine/Managers/GeometryManager.h"
#include "Engine/Components/TransformComponent.h"

class GeometryRenderer
{
public:
	static void Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry, std::shared_ptr<ComponetBufferManager> componentBufferManager);
};

