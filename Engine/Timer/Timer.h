#pragma once
#include "../EngineApi.h"
#include <chrono>

class ENGINE_API Timer
{
public:
	virtual void Update() = 0;;
	virtual float GetDeltaTime() = 0;
	virtual float GetElapsedTime() = 0;
};

