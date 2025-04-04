#include "ComponetBufferManager.h"
#include "Engine/Config.h"

void ComponetBufferManager::RegisterBuffer(const std::string& name, const Vk::BufferConfig& config)
{
	if (buffers.find(name) != buffers.end())
		return;

	buffers[name].first = config;
	buffers[name].second.resize(FRAMES_IN_FLIGHT);
}

void ComponetBufferManager::FramesInFlightChanged()
{
	for (auto& [name, buffer] : buffers)
	{
		if(FRAMES_IN_FLIGHT >= 1 && buffer.second.size() != FRAMES_IN_FLIGHT)
			buffer.second.resize(FRAMES_IN_FLIGHT);
	}
}
