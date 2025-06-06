#include "BenchmarkWindow.h"
#include "Editor/Gui/Utils/Panel.h"

void BenchmarkWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, std::set<VkDescriptorSet>& textureSet, uint32_t frameIndex)
{
	static Window::Config windowConfig{
		.name = "Benchmark-Window"
	};

	Window::RenderWindow(windowConfig, [&]() -> void
		{
			for (auto& [typeIndex, time] : resourceManager->GetBenchmarkManager()->GetAverageBenchmarkTimes())
				ImGui::Text(std::format("{} : {} ms", typeIndex.name(), time).c_str());
		}
	);
}
