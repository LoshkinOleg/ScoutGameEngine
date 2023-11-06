#pragma once

#include <vector>

#include <Scout/IGraphicsEngine.h>
#include <Scout/TypedefsAndEnums.h>

namespace Scout
{
    // TODO: align
    class Model_OpenGL33
    {
    public:
        Model_OpenGL33(std::vector<MeshHandle> meshHandles);

        ErrorCode Draw() const;

    private:
        std::vector<MeshHandle> meshes_{};
    };
}