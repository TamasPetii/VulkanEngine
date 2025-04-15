#include "BenchmarkManager.h"

BenchmarkManager::~BenchmarkManager()
{
	benchmarkTimes.clear();
}

void BenchmarkManager::ResetBenchmarkTimes()
{
	for (auto& [typeID, time] : benchmarkTimes)
		time = 0;

	for (auto& [typeID, time] : averageBenchmarkTimes)
		time = 0;
}

void BenchmarkManager::AverageBenchmarkTimes()
{
	for (auto& [typeID, time] : benchmarkTimes)
		averageBenchmarkTimes[typeID] = time / counter;
}
