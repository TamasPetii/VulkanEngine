#pragma once
#include "Engine/EngineApi.h"
#include <glm/glm.hpp>
#include <array>
#include <vector>

#include "Engine/Physics/Collider/ColliderAABB.h"
#include "Engine/Physics/Collider/ColliderOBB.h"
#include "Engine/Physics/Collider/ConvexColliderGJK.h"

class ENGINE_API BoundingVolume : public ColliderAABB, public ColliderOBB, public ConvexColliderGJK
{
protected:
	virtual void PopulateSurfacePoints() = 0;
	void GenerateBoundingVolume(const glm::vec3& minPosition, const glm::vec3& maxPosition);
	void GenerateBoundingVolumeFromSurfacePoints();
};
