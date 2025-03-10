#pragma once
#include <fstream>
#include "vk_initializers.h"
#include "vk_types.h"

class Vkpipelines
{
public:
	static bool LoadShaderModule(const std::string& filePath, VkDevice logicalDevice, VkShaderModule* outShaderModule);
};

