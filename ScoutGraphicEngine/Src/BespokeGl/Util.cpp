#include <Scout/OpenGL33/Utils_OpenGL33.h>

#include <iostream>

#include <Scout/StringUtils.h>

namespace Scout
{
    BufferContents_OpenGL33 ToOpenGL33_Enum(const BufferContents type, ErrorCode* outError)
    {
        switch (type)
        {
        case BufferContents::POSITIONS:
        {
            return BufferContents_OpenGL33::POSITIONS_VEC3F;
        }break;
        case BufferContents::MATRICES:
        {
            return BufferContents_OpenGL33::MATRICES_MAT4F;
        }break;
        case BufferContents::TEXCOORDS:
        {
            return BufferContents_OpenGL33::TEXCOORDS_VEC2F;
        }break;
        case BufferContents::NORMALS:
        {
            return BufferContents_OpenGL33::NORMALS_VEC3F;
        }break;
        case BufferContents::TANGENTS:
        {
            return BufferContents_OpenGL33::TANGENTS_VEC3F;
        }break;
        case BufferContents::BITANGENTS:
        {
            return BufferContents_OpenGL33::BITANGENTS_VEC3F;
        }break;
        case BufferContents::INDICES:
        {
            return BufferContents_OpenGL33::INDICES;
        }break;
        default:
        {
            if (outError) *outError = ErrorCode::INVALID_ENUM;
            return BufferContents_OpenGL33::POSITIONS_VEC3F;
        }break;
        }
    }

    TextureColorFormat_OpenGL33 ToOpenGL33_Enum(const TextureColorFormat format, ErrorCode* outError)
    {
        switch (format)
        {
            case TextureColorFormat::RGBA_B8:
            {
                if (outError) *outError = ErrorCode::NONE;
                return TextureColorFormat_OpenGL33::RGBA_B8;
            }break;

            default:
            {
                if (outError) *outError = ErrorCode::INVALID_ENUM;
                return TextureColorFormat_OpenGL33::NONE;
            }break;
        }
    }

    AlphaBlending_OpenGL33 ToOpenGL33_Enum(const AlphaBlending blending, ErrorCode* outError)
    {
        switch (blending)
        {
        case AlphaBlending::ALPHA:
        {
            if (outError) *outError = ErrorCode::NONE;
            return AlphaBlending_OpenGL33::ALPHA;
        }break;

        case AlphaBlending::ONE_MINUS_ALPHA:
        {
            if (outError) *outError = ErrorCode::NONE;
            return AlphaBlending_OpenGL33::ONE_MINUS_ALPHA;
        }break;

        default:
        {
            if (outError) *outError = ErrorCode::INVALID_ENUM;
            return AlphaBlending_OpenGL33::NONE;
        }break;
        }
    }

    ShadingType_OpenGL33 ToOpenGL33_Enum(const ShadingType type, ErrorCode* outError)
    {
        switch (type)
        {
        case ShadingType::NONE:
        {
            return ShadingType_OpenGL33::NONE;
        }break;
        case ShadingType::WIREFRAME:
        {
            return ShadingType_OpenGL33::WIREFRAME;
        }break;
        case ShadingType::SOLID_COLOR:
        {
            return ShadingType_OpenGL33::SOLID_COLOR;
        }break;
        case ShadingType::BASE_COLOR:
        {
            return ShadingType_OpenGL33::BASE_COLOR;
        }break;
        case ShadingType::GOOCH:
        {
            return ShadingType_OpenGL33::GOOCH;
        }break;
        default:
        {
            if (outError) *outError = ErrorCode::INVALID_ENUM;
            return ShadingType_OpenGL33::NONE;
        }break;
        }
    }

    ErrorCode ConditionTexture(
        std::uint8_t* uncompressedData, std::uint64_t& byteLen,
        const TextureColorFormat inputTextureFormat, const TextureSlot_OpenGL33 targetSlot)
    {
        if (!uncompressedData) return ErrorCode::UNEXPECTED_NULLPTR;
        if (byteLen == 0) return ErrorCode::UNEXPECTED_ARRAY_SIZE;

        switch (inputTextureFormat)
        {

            case TextureColorFormat::RGBA_B8:
            {
                switch (targetSlot)
                {

                    case TextureSlot_OpenGL33::BASE_COLOR:
                    {
                        return ErrorCode::NONE;
                    }break;

                    default:
                    {
                        return ErrorCode::UNSUPPORTED_TEXTURE_FORMAT;
                    }break;
                }
            }break;

            case TextureColorFormat::RGB_B8:
            {
                // TODO
                return ErrorCode::FEATURE_NOT_IMPLEMENTED;
            }break;

            case TextureColorFormat::RED_F32:
            {
                // TODO
                return ErrorCode::FEATURE_NOT_IMPLEMENTED;
            }break;

            default:
            {
                return ErrorCode::UNSUPPORTED_TEXTURE_FORMAT;
            }break;
        }
    }

    // TODO: make this a macro instead
    ErrorCode CheckGlErrors()
    {
        // TODO: check this is true
        // When DISABLE_GL_ERROR_CHECKS is defined, this code should optimize to "return ErrorCode::NONE;" .
        GLenum errorCode = GL_NO_ERROR;
#ifndef DISABLE_GL_ERROR_CHECKS
        errorCode = glGetError();
#endif
        if (errorCode != GL_NO_ERROR)
        {
            // TODO: check this is defined in VScode and Clion too
#ifndef NDEBUG
            // TODO: check if __FILE__ and __LINE__ is defined for VSCode and CLion.
            std::cerr << "[GL_ERROR]@" << __FILE__ << "@" << __LINE__ << ": code " << InterpretGlError(errorCode) << std::endl;
#endif
            return ErrorCode::GRAPHICAL_API_ERROR;
        }
        return ErrorCode::NONE;
    }

    std::string InterpretGlError(const GLenum errorCode, ErrorCode* outError)
    {
        // Note: error codes not using GL_X notation are those that might not be defined in OpenGL33.

        switch (errorCode)
        {
        case GL_NO_ERROR:
        {
            return "GL_NO_ERROR";
        }break;
        case GL_INVALID_ENUM:
        {
            return "GL_INVALID_ENUM";
        }break;
        case GL_INVALID_VALUE:
        {
            return "GL_INVALID_VALUE";
        }break;
        case GL_INVALID_OPERATION:
        {
            return "GL_INVALID_OPERATION";
        }break;
        case 0x0503:
        {
            return "GL_STACK_OVERFLOW";
        }break;
        case 0x0504:
        {
            return "GL_STACK_UNDERFLOW";
        }break;
        case GL_OUT_OF_MEMORY:
        {
            return "GL_OUT_OF_MEMORY";
        }break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
        {
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        }break;
        case 0x0507:
        {
            return "GL_CONTEXT_LOST";
        }break;
        case 0x8031:
        {
            return "GL_TABLE_TOO_LARGE, This errorcode is deprecated";
        }break;
        default:
        {
            return "Unhandled GL error type.";
        }break;
        }
    }

    ShadingType_OpenGL33 DeduceShadingType_(const std::string_view path, ErrorCode* outErr)
    {
        const std::string pathStr(path);
        const auto stem = ComputeStem(pathStr, outErr);

        if (stem == "wireframe") return ShadingType_OpenGL33::WIREFRAME;
        if (stem == "solidColor") return ShadingType_OpenGL33::SOLID_COLOR;
        if (stem == "baseColor") return ShadingType_OpenGL33::BASE_COLOR;
        if (stem == "gooch") return ShadingType_OpenGL33::GOOCH;
        if (stem == "putPixel") return ShadingType_OpenGL33::PUT_PIXEL;

        if (outErr) *outErr = ErrorCode::INVALID_PATH;
        return ShadingType_OpenGL33::NONE;
    }
}