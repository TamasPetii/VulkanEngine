#pragma once
#include "EngineApi.h"
#include <cstdint>

struct ENGINE_API Settings
{
	static inline constexpr uint32_t MAX_FRAMES_IN_FLIGHTS = 3;
	static inline uint32_t FRAMES_IN_FLIGHT = 2;
};