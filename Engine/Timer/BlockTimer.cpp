#include "BlockTimer.h"

BlockTimer::BlockTimer()
{
	startTime = std::chrono::high_resolution_clock::now();
}

void BlockTimer::Update()
{

}

float BlockTimer::GetDeltaTime()
{
	return 0;
}

float BlockTimer::GetElapsedTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float>(currentTime - startTime).count();
}
