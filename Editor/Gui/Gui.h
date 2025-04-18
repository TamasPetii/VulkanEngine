#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_vulkan.h"
#include "ImGui/ImGuizmo.h"
#include "Engine/Config.h"

#include <set>
#include <array>
#include <string>
#include <memory>

#include "Engine/Registry/Registry.h"
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Managers/ResourceManager.h"

struct GLFWwindow;

struct GizmoConfig
{
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::WORLD;
	bool useSnap = false;
	float snapTranslate[3] = { 1.0f, 1.0f, 1.0f };
	float snapAngle = 1.0f;
	float snapScale = 0.25f;
};

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
	void RenderGizmo(std::shared_ptr<Registry> registry);
private:
	inline static GizmoConfig gizmoConfig;

	VkImageView imageView;
	VkDescriptorPool imguiPool;
	std::array<std::set<VkDescriptorSet>, Settings::MAX_FRAMES_IN_FLIGHTS> imguiDescriptorSets;
};

