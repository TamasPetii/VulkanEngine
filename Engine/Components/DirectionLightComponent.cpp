#include "DirectionLightComponent.h"

DirectionLightShadow::DirectionLightShadow() : 
	use(false),
	textureSize(2048),
	updateFrequency(1)
{
}

DirectionLightComponent::DirectionLightComponent() : 
	color(glm::vec3(1.f)),
	direction(defaultDirectionLightDirection),
	strength(1.f),
	shininess(32)
{
}


DirectionLightGPU::DirectionLightGPU(const DirectionLightComponent& directionLightComponent) : 
	color(directionLightComponent.color),
	strength(directionLightComponent.strength),
	direction(glm::normalize(directionLightComponent.direction)),
	shininess(directionLightComponent.shininess)
{
}
