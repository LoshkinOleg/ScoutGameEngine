#pragma once

#include <vector>

#include <Scout/PreprocessorMacros.h>
#include <Scout/TypedefsAndEnums.h>

namespace Scout
{
    // TODO: align, add support for mutable shaderParams. Update shader params on binding?
    class Material_OpenGL33
    {
    public:
        Material_OpenGL33(
            const ShaderHandle shader,
            const std::vector<TextureHandle> textures,
            const std::vector<float> fShaderParams);

        void Bind() const;
        void Unbind() const;

    private:
        std::vector<float> fShaderParams_{};
        std::vector<TextureHandle> textures_{};
        ShaderHandle shader_ = ShaderHandle::INVALID_ID;
    };
}