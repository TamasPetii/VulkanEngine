#pragma once
#include "Engine/Physics/Collider/ColliderAABB.h"

class ENGINE_API TesterAABB
{
public:
	static bool Test(const ColliderAABB& A, const ColliderAABB& B);
};

