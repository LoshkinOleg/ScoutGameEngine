#include <Scout/SdlRenderer/Model.h>

#include <SDL.h>
#ifdef main
#undef main
#endif

#include <Scout/SdlRenderer/GraphicEngine.h>

namespace Scout
{
    Model_SDL::Model_SDL(const std::vector<Vec3>& vertices, const Color modelColor) :
        modelColor_(modelColor)
    {
        if (vertices_.size() % 3 != 0) throw std::runtime_error("Model_SDL::Model_SDL: vertices does not contain triangle data.");

        vertices_.resize(vertices.size());
        for (size_t i = 0; i < vertices.size(); i++)
        {
            vertices_[i] = { vertices[i].x, vertices[i].y, vertices[i].z, 1.0f };
        }
    }

    void Model_SDL::Draw(GraphicsEngine_SDL& graphicalEngine, const Mat4x4& viewMat, const Mat4x4& projMat, const Mat4x4& modelMat) const
    {
        auto* pSdlRenderer = graphicalEngine.GetSdlRenderer();
        const float viewportWidth = (float)graphicalEngine.GetViewportWidth();
        const float viewportHeight = (float)graphicalEngine.GetViewportHeight();
        const auto transformation = projMat * viewMat * modelMat;
        SDL_SetRenderDrawColor(pSdlRenderer, (Uint8)(modelColor_.r * 255.f), (Uint8)(modelColor_.g * 255.0f), (Uint8)(modelColor_.b * 255.0f), (Uint8)(modelColor_.a * 255.0f));

        const auto len = vertices_.size();
        for (size_t i = 0; i < len; i += 3)
        {
            const auto pt0_world = transformation * Vec4(vertices_[i].x, vertices_[i].y, vertices_[i].z, 0.0f);
            const auto pt1_world = transformation * Vec4(vertices_[i + 1].x, vertices_[i + 1].y, vertices_[i + 1].z, 0.0f);
            const auto pt2_world = transformation * Vec4(vertices_[i + 2].x, vertices_[i + 2].y, vertices_[i + 2].z, 0.0f);

            // To screen space following "Viewport transform" section of: https://www.khronos.org/opengl/wiki/Viewport_Transform note: adjusted to fit SDL's coordinate convention.
            const Vec2 windowPt0 =
            {
                viewportWidth * 0.5f * pt0_world.x + viewportWidth * 0.5f,
                viewportHeight - (viewportHeight * 0.5f * pt0_world.y + viewportHeight * 0.5f)
            };
            const Vec2 windowPt1 =
            {
                viewportWidth * 0.5f * pt1_world.x + viewportWidth * 0.5f,
                viewportHeight - (viewportHeight * 0.5f * pt1_world.y + viewportHeight * 0.5f)
            };
            const Vec2 windowPt2 =
            {
                viewportWidth * 0.5f * pt2_world.x + viewportWidth * 0.5f,
                viewportHeight - (viewportHeight * 0.5f * pt2_world.y + viewportHeight * 0.5f)
            };

            SDL_RenderDrawLine(
                pSdlRenderer,
                (int)(windowPt0.x),
                (int)(windowPt0.y),
                (int)(windowPt1.x),
                (int)(windowPt1.y));
            SDL_RenderDrawLine(
                pSdlRenderer,
                (int)(windowPt1.x),
                (int)(windowPt1.y),
                (int)(windowPt2.x),
                (int)(windowPt2.y));
            SDL_RenderDrawLine(
                pSdlRenderer,
                (int)(windowPt2.x),
                (int)(windowPt2.y),
                (int)(windowPt0.x),
                (int)(windowPt0.y));
        }
    }
}