#include "GlobalSettingsWindow.h"
#include "Editor/Gui/ImGui/imgui.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"

#include "Editor/Gui/Utils/Window.h"
#include "Editor/Gui/Utils/Panel.h"

void GlobalSettingsWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	static Window::Config config = Window::Config("GlobalSettings-Window");
	Window::Render(config, [&]() -> void
		{
			{ //Skybox Panel
				static Panel::Config config = Panel::Config("Skybox");
				Panel::Render(config, [&]() -> void
					{

					}
				);
			}

			{ //Physics
				static Panel::Config config = Panel::Config("Physics");
				Panel::Render(config, [&]() -> void
					{

					}
				);
			}
		}
	);
}
