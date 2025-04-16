#include "Simplex.h"

Simplex& Simplex::operator=(const std::vector<glm::vec3>& list)
{
	uint32_t index = 0;

	for (glm::vec3 point : list)
		points[index++] = point;

	size = index;

	return *this;
}

glm::vec3& Simplex::operator[](int i)
{
	return points[i];
}

void Simplex::PushFront(const glm::vec3& point)
{
	points = { point, points[0], points[1], points[2] };
	size = glm::min<uint32_t>(size + 1, 4);
}