#include "FrustumCollider.h"

FrustumFace::FrustumFace(const glm::vec3& normal, float distance) : 
	normal(glm::normalize(normal)),
	distance(distance)
{
}

FrustumFace::FrustumFace(const glm::vec3& normal, const glm::vec3 point)
{
	this->normal = glm::normalize(normal);
	this->distance = glm::dot(normal, point);
}
