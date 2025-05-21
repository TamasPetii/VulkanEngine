#include "BenchmarkManager.h"

BenchmarkManager::~BenchmarkManager()
{
	benchmarkTimes.clear();
}

void BenchmarkManager::AddToCounter()
{
	counter++;
}

void BenchmarkManager::ResetBenchmarkTimes()
{
	for (auto& [typeID, time] : benchmarkTimes)
		time = 0;

	counter = 0;
}

void BenchmarkManager::AverageBenchmarkTimes()
{
	for (auto& [typeID, time] : benchmarkTimes)
		averageBenchmarkTimes[typeID] = time / counter;
}
