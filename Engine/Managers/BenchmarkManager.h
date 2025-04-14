#pragma once
#include <unordered_map>
#include <typeindex>
#include "Engine/Registry/Unique.h"

class BenchmarkManager
{
public:
	~BenchmarkManager();

	void Update(float deltaTime);

	void ResetBenchmarkTimes();

	template<typename T>
	void AddBenchmarkTime(float time);

	template<typename T>
	float GetBenchmarkTime();
private:
	bool update = false;
	float updateTime = 0.1;
	float time = 0;
	float counter = 0;
	std::unordered_map<std::type_index, float> benchmarkTimes;
};

template<typename T>
inline void BenchmarkManager::AddBenchmarkTime(float time)
{
	benchmarkTimes[Unique::typeID<T>()] += time;
}

template<typename T>
inline float BenchmarkManager::GetBenchmarkTime()
{
	if (benchmarkTimes.find(Unique::typeID<T>()) == benchmarkTimes.end())
		return -1;

	return benchmarkTimes.at(Unique::typeID<T>());
}
