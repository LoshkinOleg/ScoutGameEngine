#pragma once

#include <cstdint>

#include <glad/glad.h>

#include <Scout/PreprocessorMacros.h>
#include <Scout/TypedefsAndEnums.h>
#include <Scout/OpenGL33/TypedefsAndEnums_OpenGL33.h>

namespace Scout
{
    // TODO: align, add support for mutable textures, mipmaps, compression
    class Texture_OpenGL33
    {
    public:
        SCOUT_NO_COPY(Texture_OpenGL33);
        Texture_OpenGL33() = delete;
        Texture_OpenGL33(Texture_OpenGL33&&);
        Texture_OpenGL33& operator=(Texture_OpenGL33&&);

        Texture_OpenGL33(
            const void* const uncompressedData, const std::uint64_t byteLen,
            const std::uint64_t width, const std::uint64_t height,
            const TextureSlot_OpenGL33 texType);
        ~Texture_OpenGL33();

        void Bind() const;
        void Unbind() const;

    private:
        std::uint32_t texGl_ = 0;
        TextureSlot_OpenGL33 textureSlot_ = TextureSlot_OpenGL33::NONE;
        bool freeVramOnDestruction_ = true;
    };
}