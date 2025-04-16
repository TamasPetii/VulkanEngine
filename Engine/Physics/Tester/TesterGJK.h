#pragma once
#include "Engine/Physics/Collider/ColliderGJK.h"
#include "Engine/Physics/Simplex.h"

class ENGINE_API TesterGJK
{
public:
	static bool Test(ColliderGJK* A, ColliderGJK* B, Simplex& simplex);
private:
	static glm::vec3 Support(ColliderGJK* A, ColliderGJK* B, const glm::vec3& direction);
	static bool SameDirection(const glm::vec3& direction, const glm::vec3& ao);
	static bool NextSimplex(Simplex& simplex, glm::vec3& direction);
	static bool LineCase(Simplex& simplex, glm::vec3& direction);
	static bool TriangleCase(Simplex& simplex, glm::vec3& direction);
	static bool TetrahedronCase(Simplex& simplex, glm::vec3& direction);
};

