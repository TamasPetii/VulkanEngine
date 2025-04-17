#include "TesterFrustum.h"

bool TesterFrustum::Test(const FrustumCollider* frustum, const ColliderAABB* box)
{
    for (const auto& face : frustum->faces)
    {
        float r = glm::dot(box->aabbExtents, glm::abs(face.normal));

        if (!(-r <= GetSignedDistance(face, box->aabbOrigin)))
            return false;
    }

    return true;
}

bool TesterFrustum::Test(const FrustumCollider* frustum, const SphereColliderGJK* sphere)
{
    for (const auto& face : frustum->faces)
    {
        if (!(GetSignedDistance(face, sphere->origin) > -sphere->radius))
            return false;
    }

    return true;
}

float TesterFrustum::GetSignedDistance(const FrustumFace& face, const glm::vec3& point)
{
    return glm::dot(face.normal, point) - face.distance;
}
