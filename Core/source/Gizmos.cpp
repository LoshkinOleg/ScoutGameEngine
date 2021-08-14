#include "Gizmos.h"

#include <glm/gtx/quaternion.hpp>

namespace sge
{
    /*void Gizmos::GizmoVector::Update(const glm::vec3 newOrigin, const glm::vec3 newEnd) const
    {
        sge_ERROR("Fix this shit");

        const float SCALE_FACTOR = 50.0f;

        glm::vec3 vec = newEnd - newOrigin;
        const float scale = glm::length(vec);
        vec = glm::normalize(vec);

        glm::mat4 model = glm::translate(IDENTITY_MAT4, newOrigin);
        const auto rotationMat = glm::toMat4(glm::quatLookAt(vec, UP_VEC3));
        model = model * rotationMat;
        model = glm::scale(model, ONE_VEC3 * scale * SCALE_FACTOR);
        *transform = model;
    }
    void Gizmos::GizmoVector::Schedule() const
    {
        auto& renderer = Engine::Get().GetRenderer();
        renderer.Schedule(model, shader, GL_LINE_STRIP);
    }

    Gizmos::GizmoVector Gizmos::CreateVector(const glm::vec3 color)
    {
        auto& rm = Engine::Get().GetResourceManager();
        auto& renderer = Engine::Get().GetRenderer();

        GizmoVector gizmo;

        std::array<glm::vec2, 5> vertices_ =
        {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.8f, 0.2f),
            glm::vec2(0.8f, -0.2f),
            glm::vec2(1.0f, 0.0f)
        };
        const std::array<float, 15> data =
        {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.8f, 0.2f, 0.0f,
            0.8f, -0.2f, 0.0f,
            1.0f, 0.0f, 0.0f
        };
        const std::array<uint32_t, 1> layout[] = { 3 };
        auto shaderSrc = rm.LoadShader("../data/shaders/gizmo.vert", "../data/shaders/gizmo.frag");

        gizmo.model = renderer.CreateModel(std::vector<float>(data.begin(), data.end()), std::vector<uint32_t>(layout->begin(), layout->end()), { IDENTITY_MAT4 }, color);
        gizmo.shader = renderer.CreateShader(shaderSrc, Shader::IlluminationModel::GIZMO);
        gizmo.transform = gizmo.model->transformsBegin;

        return gizmo;
    }*/
}//!sge