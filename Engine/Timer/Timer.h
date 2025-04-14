#pragma once
#include "../EngineApi.h"
#include <chrono>
#include <iomanip>

class ENGINE_API Timer
{
public:
	Timer();
	~Timer();
	void Update();
	float GetFrameDeltaTime() { return frameDeltaTime; }
	float GetFrameElapsedTime() { return frameElapsedTime; }
	float GetElapsedTime();
private:
	float frameDeltaTime;
	float frameElapsedTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;
};
