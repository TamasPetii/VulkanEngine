#pragma once
#include "../EngineApi.h"
#include "Entity.h"
#include <vector>

struct ENGINE_API Relationship
{
	Parent parent = NULL_ENTITY;
	std::vector<Child> children;
};

