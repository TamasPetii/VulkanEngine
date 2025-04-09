#include "Timer.h"

Timer::Timer() :
	frameDeltaTime(0),
	frameElapsedTime(0)
{
}

Timer::~Timer()
{
	startTime = std::chrono::high_resolution_clock::now();
	lastUpdateTime = startTime;
}

void Timer::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	frameDeltaTime = std::chrono::duration<double>(currentTime - lastUpdateTime).count();
	frameElapsedTime = std::chrono::duration<double>(currentTime - startTime).count();
	lastUpdateTime = currentTime;
}
