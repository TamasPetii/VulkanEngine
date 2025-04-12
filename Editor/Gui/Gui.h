#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_vulkan.h"
#include "Engine/Config.h"

#include <set>
#include <array>
#include <string>
#include <memory>

#include "Engine/Registry/Registry.h"
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Managers/ResourceManager.h"

struct GLFWwindow;

class Gui
{
public:
	Gui(GLFWwindow* window);
	~Gui();
	void Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
private:
	void Initialize(GLFWwindow* window);
	void Cleanup();
	void SetStyle();
private:
	VkImageView imageView;
	VkDescriptorPool imguiPool;
	std::array<std::set<VkDescriptorSet>, Settings::MAX_FRAMES_IN_FLIGHTS> imguiDescriptorSets;
};

