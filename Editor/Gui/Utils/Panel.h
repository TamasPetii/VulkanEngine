#pragma once
#include <string>
#include <memory>
#include <functional>
#include <optional>
#include "Editor/Gui/ImGui/imgui.h"

class Panel
{
public:
    struct Config
    {
        std::string name;
        std::optional<bool> isVisible;
        ImGuiTreeNodeFlags flags;
    };
public:
	static void Render(Config& config, const std::function<void()>& renderFunction);
};

