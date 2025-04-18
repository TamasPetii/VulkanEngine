#include "EntityWindow.h"

void EntityWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, std::set<VkDescriptorSet>& textureSet, uint32_t frameIndex)
{
	static Window::Config windowConfig{
		.name = "Entity-Window"
	};

	Window::RenderWindow(windowConfig, [&]() -> void
		{

		}
	);
}
