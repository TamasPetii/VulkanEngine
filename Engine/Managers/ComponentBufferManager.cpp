#include "ComponentBufferManager.h"
#include "Engine/Config.h"

void ComponentBufferManager::RegisterBuffer(const std::string& name, const Vk::BufferConfig& config)
{
	if (buffers.find(name) != buffers.end())
		return;

	buffers[name].first = config;
}