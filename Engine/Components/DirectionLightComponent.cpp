#include "DirectionLightComponent.h"

DirectionLightShadow::DirectionLightShadow()
{
}

DirectionLightComponent::DirectionLightComponent() : 
	direction(defaultDirectionLightDirection)
{
}


DirectionLightGPU::DirectionLightGPU(const DirectionLightComponent& directionLightComponent) : 
	color(directionLightComponent.color),
	strength(directionLightComponent.strength),
	direction(glm::normalize(directionLightComponent.direction)),
	shininess(directionLightComponent.shininess)
{
}
