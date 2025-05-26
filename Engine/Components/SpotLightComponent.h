#pragma once
#include "BaseComponents/Component.h"
#include "BaseComponents/Light.h"
#include "BaseComponents/VersionIndexed.h"
#include "BaseComponents/FrustumCullable.h"

#include <glm/glm.hpp>
#include <memory>
#include <array>

class SpotLightSystem;

struct ENGINE_API SpotLightShadow : public LightShadow
{
	SpotLightShadow();

	//farplane = length;
	glm::mat4 viewProj;
};

struct ENGINE_API SpotLightComponent : public Light, public Component, public FrustumCullable
{
	SpotLightComponent();

	glm::vec3 position; //Mapped to transform component translation!
	glm::vec3 direction; //Mapped to transform component rotation!
	glm::vec2 angles; //xy = inner/outer angle
	float length; //Mapped to transform component scale (max value)
	SpotLightShadow shadow;
	//Todo: Visible entities??? Instanced???? Maybe in models???? Or like a model map?

private:
	glm::mat4 transform; //Cannot use transform component becouse of the scale can be different on xyz!
	friend class SpotLightSystem;
};

struct ENGINE_API SpotLightGPU
{
	SpotLightGPU(const SpotLightComponent& spotLightComponent);

	glm::vec3 color;
	float strength;
	glm::vec3 position;
	float shininess;
	glm::vec3 direction;
	float length;
	glm::vec4 angles; //xy = inner/outer | zw = cos(innver)/cos(outer)
	uint32_t bitflag;
	glm::vec3 padding;
};