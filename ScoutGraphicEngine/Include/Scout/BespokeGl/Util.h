#pragma once

#include <glad/glad.h>

#include <Scout/TypedefsAndEnums.h>
#include <Scout/OpenGL33/TypedefsAndEnums_OpenGL33.h>
#include <Scout/Path.h>

namespace Scout
{
    TextureColorFormat_OpenGL33 ToOpenGL33_Enum(const TextureColorFormat format, ErrorCode* outError = nullptr);
    AlphaBlending_OpenGL33 ToOpenGL33_Enum(const AlphaBlending blending, ErrorCode* outError = nullptr);
    ShadingType_OpenGL33 ToOpenGL33_Enum(const ShadingType type, ErrorCode* outError = nullptr);
    BufferContents_OpenGL33 ToOpenGL33_Enum(const BufferContents type, ErrorCode* outError = nullptr);

    /*
        Conditions the texture for the GraphicsEngine_OpenGL33 renderer.
        Modifies uncompressedData, reallocating if need, and modifies byteLen to reflect new data format.
    */
    ErrorCode ConditionTexture(
        std::uint8_t* uncompressedData, std::uint64_t& byteLen,
        const TextureColorFormat inputTextureFormat, const TextureSlot_OpenGL33 targetSlot);
    
    ErrorCode CheckGlErrors(); // TODO: make this a macro instead
    std::string InterpretGlError(const GLenum errorCode, ErrorCode* outError = nullptr);

    ShadingType_OpenGL33 DeduceShadingType_(const std::string_view sanitizedAbsolutePath, ErrorCode* outErr = nullptr);
}