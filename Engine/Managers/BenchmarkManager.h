#pragma once
#include <unordered_map>
#include <typeindex>
#include "Engine/Registry/Unique.h"

class BenchmarkManager
{
public:
	~BenchmarkManager();

	void ResetBenchmarkTimes();
	void AverageBenchmarkTimes();

	template<typename T>
	void AddBenchmarkTime(float time);
	template<typename T>
	float GetBenchmarkTime();
	template<typename T>
	float GetAverageBenchmarkTime();
	template<typename T>
	void Register();
private:
	float counter = 0;
	std::unordered_map<std::type_index, float> benchmarkTimes;
	std::unordered_map<std::type_index, float> averageBenchmarkTimes;
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

template<typename T>
inline float BenchmarkManager::GetAverageBenchmarkTime()
{
	if (averageBenchmarkTimes.find(Unique::typeID<T>()) == averageBenchmarkTimes.end())
		return -1;

	return averageBenchmarkTimes.at(Unique::typeID<T>());
}

template<typename T>
inline void BenchmarkManager::Register()
{
	benchmarkTimes[Unique::typeID<T>()];
}
