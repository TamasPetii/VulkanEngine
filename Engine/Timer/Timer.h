#pragma once
#include "../EngineApi.h"
#include <chrono>

class ENGINE_API Timer
{
public:
	Timer();
	~Timer();
	void Update();
	double GetFrameDeltaTime() { return frameDeltaTime; }
	double GetFrameElapsedTime() { return frameElapsedTime; }

	template <typename DurationType>
	double GetElapsedTime();
private:
	double frameDeltaTime;
	double frameElapsedTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;
};

template<typename DurationType>
inline double Timer::GetElapsedTime()
{
	static_assert(std::is_convertible<DurationType, std::chrono::duration<double>>::value, "Template parameter must be a std::chrono::duration type");

	auto currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<DurationType>(currentTime - startTime).count();
}
