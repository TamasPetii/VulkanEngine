#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_vulkan.h"
#include <set>
#include <vector>

struct GLFWwindow;

class Gui
{
public:
	Gui(GLFWwindow* window);
	~Gui();
	void Render();
	void RenderDrawData(VkCommandBuffer commandBuffer);
private:
	void Initialize(GLFWwindow* window);
	void Cleanup();
	void SetStyle();
private:
	VkDescriptorPool imguiPool;
	VkImageView imageView;
	std::vector<std::set<VkDescriptorSet>> imguiDescriptorSets;
};

