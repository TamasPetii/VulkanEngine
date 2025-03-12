#pragma once
#include "../EngineApi.h"
#include "Timer.h"

class ENGINE_API FrameTimer : public Timer
{
public:
	FrameTimer();
	void Update() override;
	float GetDeltaTime() override;
	float GetElapsedTime() override;
private:
	float deltaTime;
	float elapsedTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;
};