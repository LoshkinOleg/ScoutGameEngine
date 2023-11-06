#pragma once

#include <Scout/TypedefsAndEnums.h>

namespace Scout
{
    // TODO: align
    class Mesh_OpenGL33
    {
    public:
        Mesh_OpenGL33(
            const VaoHandle vao, const VboHandle ebo,
            const std::vector<VboHandle> vertexBuffers,
            const MaterialHandle mat);

        ErrorCode Draw() const;

    private:
        VaoHandle vao_ = VaoHandle::INVALID_ID;
        EboHandle ebo_ = EboHandle::INVALID_ID;
        MaterialHandle mat_ = MaterialHandle::INVALID_ID;
        std::vector<VboHandle> vbo_{};
    };
}