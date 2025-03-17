#pragma once
#include "../EngineApi.h"
#include "Entity.h"
#include <vector>

struct ENGINE_API Relationship
{
	Relationship();

	Parent parent;
	std::vector<Child> children;
};

