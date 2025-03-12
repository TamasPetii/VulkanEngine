#pragma once
#include "../EngineApi.h"
#include "../Logger/Checker.h"
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <span>

namespace Vk
{
	class ENGINE_API ValidationLayer
	{
	public:
		static bool ValidationLayerEnabled();
		static bool CheckValidationLayerSupport();
		static std::span<const char*> GetValidationLayers();
	private:
		static bool enableValidationLayers;
		static std::vector<const char*> validationLayers;
	};
}


