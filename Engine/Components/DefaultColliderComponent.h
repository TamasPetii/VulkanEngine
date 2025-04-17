#pragma once
#include "Component.h"
#include "Engine/Physics/Collider/ColliderAABB.h"
#include "Engine/Physics/Collider/ColliderOBB.h"
#include "Engine/Physics/Collider/SphereColliderGJK.h"

struct ENGINE_API DefaultColliderComponent : public Component, public SphereColliderGJK
{
};

struct ENGINE_API DefaultCameraCollider : public ColliderAABB, public ColliderOBB
{
};