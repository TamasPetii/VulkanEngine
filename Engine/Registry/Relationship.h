#pragma once
#include "Engine/EngineApi.h"
#include "Entity.h"
#include <set>

constexpr uint32_t MAX_RELATIONSHIP_DEPTH = 16;

struct ENGINE_API Relationship
{
	uint32_t level = 0;
	Parent parent = NULL_ENTITY;
	std::set<Child> children;
};

