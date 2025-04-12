#include "BoundingVolume.h"

void BoundingVolume::GenerateBoundingVolume()
{
    glm::vec3 maxPosition{ std::numeric_limits<float>::lowest() };
    glm::vec3 minPosition{ std::numeric_limits<float>::max() };

    for (auto& position : surfacePoints)
    {
        maxPosition = glm::max(maxPosition, position);
        minPosition = glm::min(minPosition, position);
    }

    obb[0] = glm::vec3(maxPosition.x, maxPosition.y, maxPosition.z);
    obb[1] = glm::vec3(maxPosition.x, maxPosition.y, minPosition.z);
    obb[2] = glm::vec3(maxPosition.x, minPosition.y, maxPosition.z);
    obb[3] = glm::vec3(maxPosition.x, minPosition.y, minPosition.z);
    obb[4] = glm::vec3(minPosition.x, maxPosition.y, maxPosition.z);
    obb[5] = glm::vec3(minPosition.x, maxPosition.y, minPosition.z);
    obb[6] = glm::vec3(minPosition.x, minPosition.y, maxPosition.z);
    obb[7] = glm::vec3(minPosition.x, minPosition.y, minPosition.z);

    aabbMax = maxPosition;
    aabbMin = minPosition;
    aabbOrigin = 0.5f * (minPosition + maxPosition);
    aabbExtents = 0.5f * (maxPosition - minPosition);
}
