#pragma once

#include <string_view>
#include <vector>

#include "Scout/PreprocessorMacros.h"
#include "Scout/TypedefsAndEnums.h"

namespace Scout
{
    // TODO: align. Add geometry shader support, cache uniform locations
    class Shader_OpenGL33
    {
    public:
        SCOUT_NO_COPY(Shader_OpenGL33);
        Shader_OpenGL33() = default;
        Shader_OpenGL33(Shader_OpenGL33&&) noexcept;
        Shader_OpenGL33& operator=(Shader_OpenGL33&&) noexcept;
        
        Shader_OpenGL33(
            const std::string_view vertShaderSrc,
            const std::string_view fragShaderSrc);
        ~Shader_OpenGL33();

        void Bind() const;
        void Unbind();

        void SetShaderParam(const std::string_view name, const std::vector<float> value);

    private:
        std::uint32_t programGl_ = 0;
        bool freeVramOnDestruction_ = true;
    };
}