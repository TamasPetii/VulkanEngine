#pragma once
#include <memory>
#include <optional>
#include <functional>
#include "Editor/Gui/ImGui/imgui.h"
#include "Editor/Gui/ImGui/imgui_impl_vulkan.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"

class Window
{
public:
	virtual void Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, std::set<VkDescriptorSet>& textureSet, uint32_t frameIndex) = 0;
protected:
    struct Config
    {
        std::string name;
        std::optional<bool> isOpen;
        std::optional<ImVec2> size;
        std::optional<ImVec2> pos;
        ImGuiWindowFlags flags;
    };
	static void RenderWindow(Config& config, const std::function<void()>& renderFunction);
};

