#include "TesterAABB.h"

bool TesterAABB::Test(const ColliderAABB& A, const ColliderAABB& B)
{
	return (
		A.min.x <= B.max.x &&
		A.max.x >= B.min.x &&
		A.min.y <= B.max.y &&
		A.max.y >= B.min.y &&
		A.min.z <= B.max.z &&
		A.max.z >= B.min.z
		);
}
