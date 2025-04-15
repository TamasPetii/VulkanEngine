#include "BenchmarkWindow.h"
#include "Editor/Gui/ImGui/imgui.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"

#include "Editor/Gui/Utils/Window.h"
#include "Editor/Gui/Utils/Panel.h"

void BenchmarkWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	static Window::Config config = Window::Config("Benchmark-Window");
	Window::Render(config, [&]() -> void
		{
			//ImGui::Text("Physics System = ");
			//ImGui::SameLine();
			//ImGui::TextColored(ImVec4(0.75, 0.75, 0.75, 1), "%f ms", systemTimes[Unique::typeIndex<PhysicsSystem>()]);
		}
	);
}
