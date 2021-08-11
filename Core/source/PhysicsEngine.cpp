#include "PhysicsEngine.h"

namespace sge
{
    glm::vec3 Box::FullExtents() const
    {
        return end - begin;
    }
    glm::vec3 Box::HalfExtents() const
    {
        return FullExtents() * 0.5f;
    }
    glm::vec3 Box::Centroid() const
    {
        return (begin + end) * 0.5f;
    }
    bool PhysicsEngine::Intersect(const Box& box, const glm::vec3 point)
    {
        const auto relBegin = box.begin - point;
        const auto relEnd = box.end - point;
        const float projOnX_bottomLeft = glm::dot(relBegin, EAST_VEC3);
        const float projOnY_bottomLeft = glm::dot(relBegin, NORTH_VEC3);
        const float projOnZ_bottomLeft = glm::dot(relBegin, UP_VEC3);
        const float projOnX_topRight = glm::dot(relEnd, EAST_VEC3);
        const float projOnY_topRight = glm::dot(relEnd, NORTH_VEC3);
        const float projOnZ_topRight = glm::dot(relEnd, UP_VEC3);

        if((projOnX_bottomLeft > 0.0f && projOnX_topRight > 0.0f) ||
           (projOnX_bottomLeft < 0.0f && projOnX_topRight < 0.0f))
        {
            return false;
        }
        if((projOnY_bottomLeft > 0.0f && projOnY_topRight > 0.0f) ||
           (projOnY_bottomLeft < 0.0f && projOnY_topRight < 0.0f))
        {
            return false;
        }
        if((projOnZ_bottomLeft > 0.0f && projOnZ_topRight > 0.0f) ||
           (projOnZ_bottomLeft < 0.0f && projOnZ_topRight < 0.0f))
        {
            return false;
        }
        return true;
    }
}//!sge