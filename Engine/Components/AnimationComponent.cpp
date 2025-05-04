#include "AnimationComponent.h"

AnimationComponent::AnimationComponent() : 
	time(0.0),
	speed(1.f),
	animation(nullptr),
	nodeTransformBuffer(nullptr)
{
}