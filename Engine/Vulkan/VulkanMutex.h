#pragma once
#include "Engine/EngineApi.h"
#include <mutex>

class VulkanMutex
{
public:
	static inline std::mutex graphicsQueueSubmitMutex;
	static inline std::mutex transferQueueSubmitMutex;
};