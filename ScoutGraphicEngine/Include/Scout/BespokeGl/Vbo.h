#pragma once

#include <cstdint>

#include <Scout/PreprocessorMacros.h>
#include <Scout/TypedefsAndEnums.h>

namespace Scout
{
    // TODO: align, add support for mutable buffers
    class VBO_OpenGL33
    {
    public:
        SCOUT_NO_COPY(VBO_OpenGL33);
        VBO_OpenGL33() = default;
        VBO_OpenGL33(VBO_OpenGL33&&) noexcept;
        VBO_OpenGL33& operator=(VBO_OpenGL33&&) noexcept;

        VBO_OpenGL33(
            const VaoHandle vao,
            const void* const data, const std::uint64_t byteLen,
            const std::uint32_t componentsPerElement, const std::uint32_t byteSizeOfComponent,
            const BufferContents contentsType);
        ~VBO_OpenGL33();

        void Bind() const;
        static void Unbind();

    private:
        VaoHandle vao_ = VaoHandle::INVALID_ID;
        std::uint32_t vboGl_ = 0;
        bool freeVramOnDestruction_ = true;
    };
} // namespace Scout