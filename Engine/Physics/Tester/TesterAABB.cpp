#include "TesterAABB.h"

bool TesterAABB::Test(const ColliderAABB* boxA, const ColliderAABB* boxB)
{
	return (
		boxA->aabbMin.x <= boxB->aabbMax.x &&
		boxA->aabbMax.x >= boxB->aabbMin.x &&
		boxA->aabbMin.y <= boxB->aabbMax.y &&
		boxA->aabbMax.y >= boxB->aabbMin.y &&
		boxA->aabbMin.z <= boxB->aabbMax.z &&
		boxA->aabbMax.z >= boxB->aabbMin.z
		);
}

bool TesterAABB::Test(const ColliderAABB* box, const SphereColliderGJK* sphere)
{
	float x = glm::max(box->aabbMin.x, glm::min(sphere->origin.x, box->aabbMax.x));
	float y = glm::max(box->aabbMin.y, glm::min(sphere->origin.y, box->aabbMax.y));
	float z = glm::max(box->aabbMin.z, glm::min(sphere->origin.z, box->aabbMax.z));

	return glm::distance(glm::vec3(x, y, z), sphere->origin) < sphere->radius;
}

bool TesterAABB::Test(const SphereColliderGJK* sphereA, const SphereColliderGJK* sphereB)
{
	return glm::distance(sphereA->origin, sphereB->origin) < sphereA->radius + sphereB->radius;
}
