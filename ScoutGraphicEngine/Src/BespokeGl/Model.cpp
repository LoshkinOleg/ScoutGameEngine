#include <Scout/OpenGL33/Model_OpenGL33.h>

#include <Scout/OpenGL33/Mesh_OpenGL33.h>
#include <Scout/OpenGL33/GraphicsEngine_OpenGL33.h>

namespace Scout
{
    Model_OpenGL33::Model_OpenGL33(std::vector<MeshHandle> meshHandles):
        meshes_(meshHandles)
    {}

    ErrorCode Model_OpenGL33::Draw() const
    {
        for (auto& mesh : meshes_)
        {
            const auto err = GraphicsEngine_OpenGL33::Get()[mesh].Draw();
            if (err != ErrorCode::NONE) return err;
        }
        return ErrorCode::NONE;
    }
}