#include "GlobalSettingsWindow.h"
#include "Editor/Gui/Utils/Panel.h"

void GlobalSettingsWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, std::set<VkDescriptorSet>& textureSet, uint32_t frameIndex)
{
	static Window::Config windowConfig{
		.name = "GlobalSettings-Window"
	};

	Window::RenderWindow(windowConfig, [&]() -> void
		{
			static Panel::Config skyboxConfig{
				.name = "Skybox"
			};

			Panel::Render(skyboxConfig, [&]() -> void
				{

				}
			);

			static Panel::Config physicsConfig{
				.name = "Physics"
			};

			Panel::Render(physicsConfig, [&]() -> void
				{

				}
			);
		}
	);
}
