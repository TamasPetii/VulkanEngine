#pragma once
#include "Engine/EngineApi.h"
#include <stdint.h>

struct ENGINE_API Component
{
	virtual ~Component() = default;
	uint32_t versionID = 0;
};

