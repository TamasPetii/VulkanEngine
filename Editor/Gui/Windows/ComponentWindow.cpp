#include "ComponentWindow.h"
#include "Editor/Gui/Utils/Panel.h"

void ComponentWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, std::set<VkDescriptorSet>& textureSet, uint32_t frameIndex)
{
	static Window::Config windowConfig{
		.name = "Component-Window"
	};

	Window::RenderWindow(windowConfig, [&]() -> void
		{
			static Panel::Config transformConfig{
				.name = "TransformComponent",
				.isVisible = true
			};

			Panel::Render(transformConfig, [&]() -> void
				{
				}
			);
		}
	);
}
