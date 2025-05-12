#pragma once
#include "Engine/EngineApi.h"
#include "ArrayIndexedManager.h"
#include "Engine/Vulkan/Buffer.h"

template<typename T>
class DeviceAddressedManager : public ArrayIndexedManager
{
public:
	DeviceAddressedManager();
    virtual ~DeviceAddressedManager() { deviceAddresses.reset(); }
	const auto& GetDeviceAddressesBuffer() const { return deviceAddresses; }
protected:
	std::shared_ptr<Vk::Buffer> deviceAddresses;
};

template<typename T>
DeviceAddressedManager<T>::DeviceAddressedManager()
{
    //Todo: make this dynamic with animation index size 
    Vk::BufferConfig config;
    config.size = sizeof(T) * 1024;
    config.usage = VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
    config.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    deviceAddresses = std::make_shared<Vk::Buffer>(config);
    deviceAddresses->MapMemory();
}
