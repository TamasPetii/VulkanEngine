#pragma once
#include "../EngineApi.h"
#include <chrono>

class ENGINE_API Timer
{
public:
	Timer();
	~Timer();
	void Update();
	float GetFrameDeltaTime() { return frameDeltaTime; }
	float GetFrameElapsedTime() { return frameElapsedTime; }

	template <typename DurationType>
	float GetElapsedTime();
private:
	float frameDeltaTime;
	float frameElapsedTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;
};

template<typename DurationType>
inline float Timer::GetElapsedTime()
{
	static_assert(std::is_convertible<DurationType, std::chrono::duration<float>>::value, "Template parameter must be a std::chrono::duration type");

	auto currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<DurationType>(currentTime - startTime).count();
}
