#pragma once
#include "../EngineApi.h"
#include "Entity.h"
#include <vector>
#include <bitset>

template<uint32_t MAX_COMPONENTS>
struct ComponentBitset
{
	std::bitset<MAX_COMPONENTS> bitset;
};