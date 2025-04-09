#pragma once
#include <string>
#include <functional>
#include <memory>
#include "Editor/Gui/ImGui/imgui.h"

class Window
{
public:
    struct Config
    {
        Config();
        Config(const std::string& name, std::unique_ptr<bool> isOpen = nullptr, ImGuiWindowFlags flags = ImGuiWindowFlags_None);

        std::string name;
        std::unique_ptr<bool> isOpen;
        std::pair<bool, ImVec2> size;
        std::pair<bool, ImVec2> position;
        ImGuiWindowFlags flags;
    };
public:
	static void Render(Config& config, const std::function<void()>& renderFunction);
};

