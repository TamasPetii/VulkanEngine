#pragma once
#include "Entity.h"
#include <vector>

struct Relationship
{
	Relationship();

	Parent parent;
	std::vector<Child> children;
};

