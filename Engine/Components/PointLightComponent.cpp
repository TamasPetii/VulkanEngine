#include "PointLightComponent.h"

PointLightShadow::PointLightShadow() :
	use(false),
	textureSize(2048),
	updateFrequency(1)
{
}

PointLightComponent::PointLightComponent() :
	color(glm::vec3(1.f)),
	position(glm::vec3(0.f)),
	radius(1.f),
	weakenDistance(0.f),
	strength(1.f),
	shininess(32),
	transform(glm::mat4(1))
{
}


PointLightGPU::PointLightGPU(const PointLightComponent& pointLightComponent) :
	color(pointLightComponent.color),
	strength(pointLightComponent.strength),
	position(pointLightComponent.position),
	shininess(pointLightComponent.shininess),
	radius(pointLightComponent.radius),
	weakenDistance(pointLightComponent.weakenDistance),
	bitflag(0)
{
	bitflag |= (pointLightComponent.shadow.use ? 1u : 0u) << 0; // Bit 0 = Simulate Shadow?
}
