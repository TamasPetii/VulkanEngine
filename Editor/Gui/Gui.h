#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_vulkan.h"

struct GLFWwindow;

class Gui
{
public:
	Gui(GLFWwindow* window);
	~Gui();
	void Render(VkCommandBuffer commandBuffer);
private:
	void Initialize(GLFWwindow* window);
	void Cleanup();
	void SetStyle();
private:
	VkDescriptorPool imguiPool;
	VkDescriptorSet imageDescriptorSet;
	VkImageView imageView;
};

