#pragma once

#include <GL/gl.h>

namespace Scout
{
    /*
        Number matches attribute's shader location for applicable content types.
    */
    enum class BufferContents_OpenGL33 : GLuint
    {
        POSITIONS_VEC3F = 0,
        MATRICES_MAT4F = 1,
        // 1 through 4 is occupied by the model matrix
        TEXCOORDS_VEC2F = 5,
        NORMALS_VEC3F = 6,
        TANGENTS_VEC3F = 7,
        BITANGENTS_VEC3F = 8,

        INDICES,
    };

    enum class ShadingType_OpenGL33 : std::uint64_t
    {
        NONE = 0,

        WIREFRAME,
        SOLID_COLOR,
        BASE_COLOR,
        GOOCH,
        PUT_PIXEL,
    };

    enum class TextureColorFormat_OpenGL33: GLenum
    {
        NONE = 0,

        RGBA_B8 = GL_RGBA8,
    };

    enum class TextureSampling_OpenGL33: GLenum
	{
		NONE = 0,

		NEAREST = GL_NEAREST, // GL_NEAREST
		LINEAR = GL_LINEAR, // GL_LINEAR
        // TODO: mip maps sampling
	};

    enum class TextureWrapping_OpenGL33: GLenum
	{
		NONE = 0,

		CLAMP = GL_CLAMP_TO_EDGE,
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT
	};

    enum class TextureCompression_OpenGL33 : GLenum
    {
        NONE = 0,

        // TODO: implement compressed textures support
    };

    enum class AlphaBlending_OpenGL33: GLenum
	{
		NONE = 0,

        ALPHA = GL_SRC_ALPHA,
        ONE_MINUS_ALPHA = GL_ONE_MINUS_SRC_ALPHA
	};
    
    enum class TextureSlot_OpenGL33: GLenum
    {
        NONE = 0,

        TEX_0 = GL_TEXTURE0,
        TEX_1 = GL_TEXTURE0 + 1,
        TEX_2 = GL_TEXTURE0 + 2,
        TEX_3 = GL_TEXTURE0 + 3,
        TEX_4 = GL_TEXTURE0 + 4,
        TEX_5 = GL_TEXTURE0 + 5,
        TEX_6 = GL_TEXTURE0 + 6,
        TEX_7 = GL_TEXTURE0 + 7,
        TEX_8 = GL_TEXTURE0 + 8,
        TEX_9 = GL_TEXTURE0 + 9,
        TEX_10 = GL_TEXTURE0 + 10,
        TEX_11 = GL_TEXTURE0 + 11,
        TEX_12 = GL_TEXTURE0 + 12,
        TEX_13 = GL_TEXTURE0 + 13,
        TEX_14 = GL_TEXTURE0 + 14,
        TEX_15 = GL_TEXTURE0 + 15, // Minimal number of texture slots defined by OpenGL3

        BASE_COLOR = TEX_0, // RGBA, 1 byte per element

        // TODO: cube maps?
    };
} // namespace Scout