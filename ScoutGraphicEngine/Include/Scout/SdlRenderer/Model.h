#pragma once

#include <vector>

#include <Scout/Color.h>
#include <Scout/Math.h>

namespace Scout
{
    class GraphicsEngine_SDL;

    class Model_SDL
    {
    public:
        /*
        @vertices Unindexed vertex positions of triangles composing this mesh.
        */
        Model_SDL(const std::vector<Vec3>& vertices, const Color modelColor);

        void Draw(GraphicsEngine_SDL& graphicalEngine, const Mat4x4& viewMat, const Mat4x4& projMat, const Mat4x4& modelMat) const;

    private:
        std::vector<Vec4> vertices_{};
        Color modelColor_ = COLOR_CLEAR;
    };
}