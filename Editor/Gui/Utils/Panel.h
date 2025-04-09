#pragma once
#include <string>
#include <functional>
#include <memory>
#include "Editor/Gui/ImGui/imgui.h"

class Panel
{
public:
    struct Config
    {
        Config();
        Config(const std::string& name, std::unique_ptr<bool> isOpen = nullptr, ImGuiWindowFlags flags = ImGuiTreeNodeFlags_DefaultOpen);

        std::string name;
        std::unique_ptr<bool> isOpen;
        ImGuiTreeNodeFlags flags;
    };
public:
	static void Render(Config& config, const std::function<void()>& renderFunction);
};

