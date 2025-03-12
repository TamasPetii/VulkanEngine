#include "FrameTimer.h"

FrameTimer::FrameTimer() :
	deltaTime(0),
	elapsedTime(0)
{
	startTime = std::chrono::high_resolution_clock::now();
	lastUpdateTime = startTime;
}

void FrameTimer::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(currentTime - lastUpdateTime).count();
	elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();
	lastUpdateTime = currentTime;
}

float FrameTimer::GetDeltaTime()
{
	return deltaTime;
}

float FrameTimer::GetElapsedTime()
{
	return elapsedTime;
}
