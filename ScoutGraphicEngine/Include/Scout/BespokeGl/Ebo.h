#pragma once

#include <cstdint>

#include <Scout/PreprocessorMacros.h>
#include <Scout/IGraphicsEngine.h>

namespace Scout
{
    // TODO: align, add support for mutable buffers
    class EBO_OpenGL33
    {
    public:
        SCOUT_NO_COPY(EBO_OpenGL33);
        EBO_OpenGL33() = default;
        EBO_OpenGL33(EBO_OpenGL33&&);
        EBO_OpenGL33& operator=(EBO_OpenGL33&&);

        EBO_OpenGL33(
            const VaoHandle vao,
            const void* const data, const std::uint64_t byteLen,
            const std::uint32_t byteSizeOfIndex);
        ~EBO_OpenGL33();

        void Bind() const;
        static void Unbind();

    private:
        VaoHandle vao_ = VaoHandle::INVALID_ID;
        std::uint32_t eboGl_ = 0;
        bool freeVramOnDestruction_ = true;
    };
} // namespace Scout