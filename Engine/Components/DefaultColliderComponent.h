#pragma once
#include "Component.h"
#include "Engine/Physics/Collider/ColliderAABB.h"
#include "Engine/Physics/Collider/ColliderOBB.h"

struct DefaultColliderComponent : public Component, public ColliderAABB, public ColliderOBB
{
};

