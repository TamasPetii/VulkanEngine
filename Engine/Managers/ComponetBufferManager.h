#pragma once
#include "Engine/Vulkan/Buffer.h"
#include <unordered_map>
#include <string>
#include <memory>

class Vk::Buffer;

struct ComponentBuffer
{
	std::shared_ptr<Vk::Buffer> buffer;
	std::vector<uint32_t> versions;
};

class ComponetBufferManager
{
public:
	void FramesInFlightChanged();
	void RegisterBuffer(const std::string& name, const Vk::BufferConfig& config);
	auto* GetComponentBuffer(const std::string& name, uint32_t framesInFlightIndex) { return &(buffers.at(name).second[framesInFlightIndex]); }

	template<typename T>
	void RecreateBuffer(const std::string& name, uint32_t size, uint32_t frameInFlightIndex);
private:
	uint32_t bufferBlockSize = 256;
	std::unordered_map<std::string, std::pair<Vk::BufferConfig, std::vector<ComponentBuffer>>> buffers;
};

template<typename T>
inline void ComponetBufferManager::RecreateBuffer(const std::string& name, uint32_t size, uint32_t frameInFlightIndex)
{
	if (buffers.find(name) == buffers.end())
		return;

	auto& [config, buffer] = buffers[name];
	uint32_t requiredSize = static_cast<uint32_t>(std::ceil(size / (float)bufferBlockSize)) * bufferBlockSize;
	auto& componentBuffer = buffer[frameInFlightIndex];

	if (componentBuffer.versions.size() != requiredSize)
	{
		componentBuffer.versions.clear();
		componentBuffer.versions.resize(requiredSize, 0);

		config.size = requiredSize * sizeof(T);
		componentBuffer.buffer = std::make_shared<Vk::Buffer>(config);
	}
}
