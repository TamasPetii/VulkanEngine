#include "Window.h"

void Window::RenderWindow(Config& config, const std::function<void()>& renderFunction)
{
	if (config.pos.has_value())
		ImGui::SetNextWindowPos(config.pos.value());

	if (config.size.has_value())
		ImGui::SetNextWindowSize(config.size.value());

	if (ImGui::Begin(config.name.c_str(), config.isOpen.has_value() ? &config.isOpen.value() : nullptr, config.flags))
		if(renderFunction) renderFunction();


	ImGui::End();
}
