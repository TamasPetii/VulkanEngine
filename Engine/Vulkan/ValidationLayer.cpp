#include "ValidationLayer.h"

/*
#ifdef NDEBUG
bool Vk::ValidationLayer::enableValidationLayers = false;
std::vector<const char*> Vk::ValidationLayer::validationLayers = {};
#else
bool Vk::ValidationLayer::enableValidationLayers = true;
std::vector<const char*> Vk::ValidationLayer::validationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif
*/

bool Vk::ValidationLayer::enableValidationLayers = true;
std::vector<const char*> Vk::ValidationLayer::validationLayers = { "VK_LAYER_KHRONOS_validation" };

bool Vk::ValidationLayer::ValidationLayerEnabled()
{
	return enableValidationLayers;
}

bool Vk::ValidationLayer::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			std::cout << "Validation Layer not found: " + std::string(layerName) << std::endl;
			return false;
		}
	}

	return true;
}

std::span<const char*> Vk::ValidationLayer::GetValidationLayers()
{
	return validationLayers; 
}
