#pragma once
#include "Engine/Physics/Collider/ColliderAABB.h"
#include "Engine/Physics/Collider/SphereColliderGJK.h"
#include "Engine/Physics/Collider/FrustumCollider.h"

class ENGINE_API TesterFrustum
{
public:
	static bool Test(const FrustumCollider* frustum, const ColliderAABB* box);
	static bool Test(const FrustumCollider* frustum, const SphereColliderGJK* sphere);
private:
	static float GetSignedDistance(const FrustumFace& face, const glm::vec3& point);
};

