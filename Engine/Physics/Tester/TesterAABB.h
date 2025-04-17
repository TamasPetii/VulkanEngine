#pragma once
#include "Engine/Physics/Collider/ColliderAABB.h"
#include "Engine/Physics/Collider/SphereColliderGJK.h"

class ENGINE_API TesterAABB
{
public:
	static bool Test(const ColliderAABB* boxA, const ColliderAABB* boxB);
	static bool Test(const ColliderAABB* box, const SphereColliderGJK* sphere);
	static bool Test(const SphereColliderGJK* sphereA, const SphereColliderGJK* sphereB);
};

