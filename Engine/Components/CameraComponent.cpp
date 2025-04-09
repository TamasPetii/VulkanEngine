#include "CameraComponent.h"

CameraComponentGPU::CameraComponentGPU(const CameraComponent& component) : 
	view(component.view),
	viewInv(component.viewInv),
	proj(component.proj),
	projInv(component.projInv),
	viewProj(component.viewProj),
	viewProjInv(component.viewProjInv),
	eye(component.position, 0.f)
{
}
