#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_vulkan.h"

#include <set>
#include <array>
#include <string>
#include <memory>

#include "Engine/Config.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Managers/ResourceManager.h"
#include "Windows/Window.h"

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
	std::array<std::set<VkDescriptorSet>, GlobalConfig::FrameConfig::maxFramesInFlights> imguiDescriptorSets;
	std::unordered_map<std::string, std::shared_ptr<Window>> windows;
};

