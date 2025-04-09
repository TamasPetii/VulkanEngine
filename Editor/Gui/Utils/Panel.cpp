#include "Panel.h"

Panel::Config::Config() : Config("", nullptr, ImGuiTreeNodeFlags_DefaultOpen)
{
}

Panel::Config::Config(const std::string& name, std::unique_ptr<bool> isOpen, ImGuiWindowFlags flags) :
	name(name),
	isOpen(std::move(isOpen)),
	flags(flags)
{
}

void Panel::Render(Config& config, const std::function<void()>& renderFunction)
{
	if (ImGui::CollapsingHeader(config.name.c_str(), config.isOpen.get(), config.flags))
	{
	}
}
