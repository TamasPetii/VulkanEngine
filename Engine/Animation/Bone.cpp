#include "Bone.h"
#include "Engine/Renderable/Model/AssimpConverter.h"
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_transform.hpp>

Bone::Bone(aiNodeAnim* channel)
{
	scales.reserve(channel->mNumScalingKeys);
	positions.reserve(channel->mNumPositionKeys);
	rotations.reserve(channel->mNumRotationKeys);
	
	ProcessScaleKeys(channel);
	ProcessPositionKeys(channel);
	ProcessRotationKeys(channel);
}

void Bone::ProcessScaleKeys(aiNodeAnim* channel)
{
	for (uint32_t i = 0; i < channel->mNumScalingKeys; ++i)
		scales.emplace_back(KeyScale(Assimp::ConvertAssimpToGlm(channel->mScalingKeys[i].mValue), channel->mScalingKeys[i].mTime));
}

void Bone::ProcessPositionKeys(aiNodeAnim* channel)
{
    for (uint32_t i = 0; i < channel->mNumPositionKeys; ++i)
        positions.emplace_back(KeyPosition(Assimp::ConvertAssimpToGlm(channel->mPositionKeys[i].mValue), channel->mPositionKeys[i].mTime));
}

void Bone::ProcessRotationKeys(aiNodeAnim* channel)
{
	for (uint32_t i = 0; i < channel->mNumRotationKeys; ++i)
		rotations.emplace_back(KeyRotation(Assimp::ConvertAssimpToGlm(channel->mRotationKeys[i].mValue), channel->mRotationKeys[i].mTime));
}

uint32_t Bone::GetScaleKeyIndex(double time) const
{
	for (uint32_t i = 0; i < scales.size() - 1; ++i)
	{
		if (time < scales[i + 1].timeStamp)
			return i;
	}

	return UINT32_MAX;
}

uint32_t Bone::GetPositionKeyIndex(double time) const
{
	for (uint32_t i = 0; i < positions.size() - 1; ++i)
	{
		if (time < positions[i + 1].timeStamp)
			return i;
	}

	return UINT32_MAX;
}

uint32_t Bone::GetRotationKeyIndex(double time) const
{
	for (uint32_t i = 0; i < rotations.size() - 1; ++i)
	{
		if (time < rotations[i + 1].timeStamp)
			return i;
	}

	return UINT32_MAX;
}

glm::mat4 Bone::InterpolateScale(double time) const
{
	if (scales.size() == 1)
		return glm::scale(scales[0].scale);

	int p0Index = GetScaleKeyIndex(time);
	int p1Index = p0Index + 1;
	double factor = GetFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, time);
	glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, static_cast<float>(factor));
	return glm::scale(finalScale);
}

glm::mat4 Bone::InterpolatePosition(double time) const
{
	if (positions.size() == 1)
		return glm::translate(positions[0].position);

	int p0Index = GetPositionKeyIndex(time);
	int p1Index = p0Index + 1;
	double factor = GetFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, time);
	glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, static_cast<float>(factor));
	return glm::translate(finalPosition);
}

glm::mat4 Bone::InterpolateRotation(double time) const
{
	if (rotations.size() == 1)
		return glm::toMat4(glm::normalize(rotations[0].orientation));

	int p0Index = GetRotationKeyIndex(time);
	int p1Index = p0Index + 1;
	double factor = GetFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, time);
	glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation, static_cast<float>(factor));
	return glm::toMat4(glm::normalize(finalRotation));
}

double Bone::GetFactor(double lastTimeStamp, double nextTimeStamp, double time) const
{
	return (time - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);
}

glm::mat4 Bone::GetTransform(double time) const
{
	return InterpolatePosition(time) * InterpolateRotation(time) * InterpolateScale(time);
}
