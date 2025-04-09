#include "EntityWindow.h"
#include "Editor/Gui/ImGui/imgui.h"
#include "Editor/Gui/Utils/Window.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"

void EntityWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	static Window::Config config = Window::Config("Entity-Window");
	Window::Render(config, [&]() -> void
		{

		}
	);
}
