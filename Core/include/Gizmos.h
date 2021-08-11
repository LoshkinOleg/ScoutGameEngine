#pragma once

#include "Engine.h"

namespace sge
{
    class Gizmos
    {
    public: // TODO: encapsulate this in a container, resource, handle too.
        struct GizmoVector
        {
            ModelHandle model = {};
            ShaderHandle shader = {};
            glm::mat4* transform = nullptr;

            void Update(const glm::vec3 newOrigin, const glm::vec3 newEnd) const;
            void Schedule() const; // Get rid of this shit. User should know how to draw the object.
        };

        static GizmoVector CreateVector(const glm::vec3 color);
    };
}//!sge