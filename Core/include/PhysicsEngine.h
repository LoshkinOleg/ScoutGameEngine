#pragma once

#include "globals.h"

namespace sge
{
    struct Box
    {
        glm::vec3 FullExtents() const;
        glm::vec3 HalfExtents() const;
        glm::vec3 Centroid() const;

        glm::vec3 begin = ZERO_VEC3;
        glm::vec3 end = ZERO_VEC3;
    };

    class PhysicsEngine
    {
    public:
        static bool Intersect(const Box& box, const glm::vec3 point);
    };

}//!sge