#include <Scout/OpenGL33/Mesh_OpenGL33.h>

namespace Scout
{
    Mesh_OpenGL33::Mesh_OpenGL33(
        const VaoHandle vao, const VboHandle ebo,
        const std::vector<VboHandle> vertexBuffers,
        const MaterialHandle mat):
            vao_(vao), ebo_(ebo), mat_(mat), vbo_(vertexBuffers)
    {}

    ErrorCode Mesh_OpenGL33::Draw() const
    {
        // TODO
        return ErrorCode::FEATURE_NOT_IMPLEMENTED;
    }
}