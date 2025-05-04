#pragma once
#include "Engine/EngineApi.h"
#include <Assimp/types.h>
#include <Assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

class ENGINE_API Bone
{
private:
    struct Key 
    {
        double timeStamp;
    };

    struct KeyPosition : public Key
    {
        KeyPosition(const glm::vec3& position, double timeStamp) : Key(timeStamp), position(position) {}

        glm::vec3 position;
    };

    struct KeyRotation : public Key
    {
        KeyRotation(const glm::quat& orientation, double timeStamp) : Key(timeStamp), orientation(orientation) {}

        glm::quat orientation;
    };

    struct KeyScale : public Key
    {
        KeyScale(const glm::vec3& scale, double timeStamp) : Key(timeStamp), scale(scale) {}

        glm::vec3 scale;
    };
public:
    Bone() = default;
    Bone(aiNodeAnim* channel);
    glm::mat4 GetTransform(double time) const;
private:
    void ProcessScaleKeys(aiNodeAnim* channel);
    void ProcessPositionKeys(aiNodeAnim* channel);
    void ProcessRotationKeys(aiNodeAnim* channel);
    uint32_t GetScaleKeyIndex(double time) const;
    uint32_t GetPositionKeyIndex(double time) const;
    uint32_t GetRotationKeyIndex(double time) const;
    glm::mat4 InterpolateScale(double time) const;
    glm::mat4 InterpolatePosition(double time) const;
    glm::mat4 InterpolateRotation(double time) const;
    double GetFactor(double lastTimeStamp, double nextTimeStamp, double time) const;
private:
    std::vector<KeyScale> scales;
    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
};

