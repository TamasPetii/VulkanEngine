#include "MaterialComponent.h"

MaterialComponent::MaterialComponent() :
	useBloom(false),
	color(1, 1, 1, 1),
	uvScale(1, 1),
	albedo(nullptr),
	normal(nullptr),
	metallic(nullptr),
	roughness(nullptr)
{
}

MaterialComponentGPU::MaterialComponentGPU(const MaterialComponent& component) :
	color(component.color),
	uvScale(component.uvScale),
	bloom(component.useBloom ? 1 : 0, 0),
	albedoIndex(component.albedo ? component.albedo->GetDescriptorArrayIndex() : UINT32_MAX),
	normalIndex(component.normal ? component.normal->GetDescriptorArrayIndex() : UINT32_MAX),
	metallicIndex(component.metallic ? component.metallic->GetDescriptorArrayIndex() : UINT32_MAX),
	roughnessIndex(component.roughness ? component.roughness->GetDescriptorArrayIndex() : UINT32_MAX)
{

}