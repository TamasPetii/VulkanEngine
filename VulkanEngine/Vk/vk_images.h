#pragma once
#include "vk_initializers.h"

class Vkimages
{
public:
	static void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
};

