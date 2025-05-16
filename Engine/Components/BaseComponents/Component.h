#pragma once
#include "Engine/EngineApi.h"
#include "VersionIndexed.h"

struct ENGINE_API Component : public VersionIndexed
{
	virtual ~Component() = default;
};
