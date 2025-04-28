#pragma once
#include "../EngineApi.h"
#include "Entity.h"
#include <vector>

constexpr uint32_t MAX_RELATIONSHIP_DEPTH = 8;

struct ENGINE_API Relationship
{
	uint32_t level = 0;
	Parent parent = NULL_ENTITY;
	std::vector<Child> children;
};

