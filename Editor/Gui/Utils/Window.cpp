#include "Window.h"

Window::Config::Config() : Config("", nullptr, ImGuiWindowFlags_None)
{
}

Window::Config::Config(const std::string& name, std::unique_ptr<bool> isOpen, ImGuiWindowFlags flags) :
	name(name),
	isOpen(std::move(isOpen)),
	size(std::make_pair(false, ImVec2(0, 0))),
	position(std::make_pair(false, ImVec2(0, 0))),
	flags(flags)
{
}

void Window::Render(Config& config, const std::function<void()>& renderFunction)
{
	if (config.position.first) {
		ImGui::SetNextWindowPos(config.position.second);
	}
	if (config.size.first) {
		ImGui::SetNextWindowSize(config.size.second);
	}

	if (ImGui::Begin(config.name.c_str(), config.isOpen.get(), config.flags))
	{
		renderFunction();
	}

	ImGui::End();
}
