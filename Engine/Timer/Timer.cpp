#include "Timer.h"

Timer::Timer() :
	frameDeltaTime(0),
	frameElapsedTime(0)
{
	startTime = std::chrono::high_resolution_clock::now();
	lastUpdateTime = startTime;
}

Timer::~Timer()
{
}

void Timer::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	frameDeltaTime = std::chrono::duration<float>(currentTime - lastUpdateTime).count();
	frameElapsedTime = std::chrono::duration<float>(currentTime - startTime).count();
	lastUpdateTime = currentTime;
}

float Timer::GetElapsedTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float, std::milli>(currentTime - startTime).count();
}
