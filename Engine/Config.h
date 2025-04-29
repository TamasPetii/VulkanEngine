#pragma once
#include "EngineApi.h"
#include <cstdint>

struct ENGINE_API GlobalConfig
{
	struct ENGINE_API WireframeConfig
	{
		static inline bool showColliderAABB = false;
		static inline bool showColliderOBB = false;
		static inline bool showColliderSphere = false;
	};

	struct ENGINE_API FrameConfig
	{
		static inline constexpr uint32_t maxFramesInFlights = 3;
		static inline uint32_t framesInFlight = 2;
	};
};