#include "BenchmarkManager.h"

BenchmarkManager::~BenchmarkManager()
{
	benchmarkTimes.clear();
}

void BenchmarkManager::Update(float deltaTime)
{
	if()
}

void BenchmarkManager::ResetBenchmarkTimes()
{
	for (auto& [typeID, time] : benchmarkTimes)
		time = 0;
}
