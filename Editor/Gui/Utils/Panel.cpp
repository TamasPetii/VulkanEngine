#include "Panel.h"

void Panel::Render(Config& config, const std::function<void()>& renderFunction)
{
	if (ImGui::CollapsingHeader(config.name.c_str(), config.isVisible.has_value() ? &config.isVisible.value() : nullptr, config.flags))
	{
		if (renderFunction)
			renderFunction();
	}
}
