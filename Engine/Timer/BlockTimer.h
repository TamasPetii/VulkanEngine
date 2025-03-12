#pragma once
#include "../EngineApi.h"
#include "Timer.h"

class ENGINE_API BlockTimer : public Timer
{
public:
	BlockTimer();
	void Update() override;
	float GetDeltaTime() override;
	float GetElapsedTime() override;
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};
