#include "TesterAABB.h"

bool TesterAABB::Test(const ColliderAABB& A, const ColliderAABB& B)
{
	return (
		A.aabbMin.x <= B.aabbMax.x &&
		A.aabbMax.x >= B.aabbMin.x &&
		A.aabbMin.y <= B.aabbMax.y &&
		A.aabbMax.y >= B.aabbMin.y &&
		A.aabbMin.z <= B.aabbMax.z &&
		A.aabbMax.z >= B.aabbMin.z
		);
}
