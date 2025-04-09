#include "ComponentWindow.h"
#include "Editor/Gui/ImGui/imgui.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"

#include "Editor/Gui/Utils/Window.h"
#include "Editor/Gui/Utils/Panel.h"

void ComponentWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	static Window::Config config = Window::Config("Component-Window");
	Window::Render(config, [&]() -> void
		{
			static Panel::Config config = Panel::Config("TransformComponent", std::make_unique<bool>(true));
			Panel::Render(config, [&]() -> void
				{

				}
			);
		}
	);
}
