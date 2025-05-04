#include "DescriptorArrayIndexed.h"

uint32_t DescriptorArrayIndexed::GetAvailableIndex()
{
	std::unique_lock<std::mutex> lock(availableIndexMutex);

	if (availableIndices.empty())
		return counter++;

	uint32_t index = *availableIndices.begin();
	availableIndices.erase(index);
	return index;
}
