#include <Scout/OpenGL33/Material_OpenGL33.h>

#include <Scout/OpenGL33/GraphicsEngine_OpenGL33.h>

namespace Scout
{
    Material_OpenGL33::Material_OpenGL33(
        const ShaderHandle shader,
        const std::vector<TextureHandle> textures,
        const std::vector<float> fShaderParams) :
            fShaderParams_(fShaderParams), textures_(textures), shader_(shader)
    {}

    void Material_OpenGL33::Bind() const
    {
        GraphicsEngine_OpenGL33::Get()[shader_].Bind();
        for (auto& tex : textures_)
        {
            GraphicsEngine_OpenGL33::Get()[tex].Bind();
        }
    }

    void Material_OpenGL33::Unbind() const
    {
        GraphicsEngine_OpenGL33::Get()[shader_].Unbind();
        for (auto& tex : textures_)
        {
            GraphicsEngine_OpenGL33::Get()[tex].Unbind();
        }
    }
}