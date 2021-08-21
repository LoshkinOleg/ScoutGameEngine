#pragma once

namespace sge
{
	enum class E_ColorFormat: unsigned
	{
		INVALID = 0,

		RGBA_B8 = 0x8058, // GL_RGBA8
		RED_F32 = 0x822E // GL_R32F
	};
	enum class E_Mutability: unsigned
	{
		INVALID = 0,

		STATIC = 0x88E4, // GL_STATIC_DRAW
		DYNAMIC = 0x88E8 // GL_DYNAMIC_DRAW
	};
	enum class E_NumberType: unsigned
	{
		INVALID = 0,

		UINT = 0x1405, // GL_UNSIGNED_INT
		FLOAT = 0x1406 // GL_FLOAT
	};
	enum class E_DrawingPrimitive: unsigned
	{
		POINTS = 0, // GL_POINTS
		LINES = 1, // GL_LINES
		LINE_STRIP = 3, // GL_LINE_STRIP
		TRIANGLES = 4, // GL_TRIANGLES
	};
	enum class E_VertexBufferTarget: unsigned
	{
		INVALID = 0,

		VBO = 0x8892, // GL_ARRAY_BUFFER
		EBO = 0x8893 // GL_ELEMENT_ARRAY_BUFFER
	};
	enum class E_SamplingMode: unsigned
	{
		INVALID = 0,

		NEAREST = 0x2600, // GL_NEAREST
		LINEAR = 0x2601, // GL_LINEAR
		LINEAR_MIPMAP_NEAREST = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
		LINEAR_MIPMAP_LINEAR = 0x2703 // GL_LINEAR_MIPMAP_LINEAR
	};
	enum class E_WrappingMode: unsigned
	{
		INVALID = 0,

		CLAMP = 0x812F, // GL_CLAMP_TO_EDGE
		REPEAT = 0x2901, // GL_REPEAT
		MIRRORED_REPEAT = 0x8370 // GL_MIRRORED_REPEAT
	};
	enum class E_TextureCompressionMode: unsigned
	{
		INVALID = 0,

		NONE = 1,
		ETC1 = 0x8D64, // GL_COMPRESSED_RGB8_ETC1
		ETC2 = 0x9278, // GL_COMPRESSED_RGBA8_ETC2_EAC
		ASTC_RGBA_4x4 = 0x93B0 // GL_COMPRESSED_RGBA_ASTC_4x4_KHR
	};

	enum F_ShadingMode: unsigned char
	{
		INVALID = 0,

		GIZMO = 1 << 0,
		ALBEDO_ONLY = 1 << 1,
		GOOCH = 1 << 2,
		BLINN_PHONG = 1 << 3,

		BLINN_PHONG_NORMALMAPPED = 1 << 4,
		SHADOW_PASS = 1 << 5,
		POST_PROCESS_PASS = 1 << 6,
		DEFERRED_PASS = 1 << 7
	};
	enum F_TextureType: unsigned char
	{
		INVALID = 0,

		ALBEDO_MAP = 1 << 0,
		SPECULAR_MAP = 1 << 1,
		NORMAL_MAP = 1 << 2,
		HEIGHT_MAP = 1 << 3,
	};
	enum F_VertexBufferType: unsigned short
	{
		INVALID = 0,

		POSITIONS_VEC3 = 1 << 0,
		POSITIONS_VEC2 = 1 << 1,
		NORMALS = 1 << 2,
		TANGENTS = 1 << 3,

		BITANGENTS = 1 << 4,
		UVS = 1 << 5,
		INDICES_UINT32 = 1 << 6,
		MODEL_MATRIX = 1 << 7,

		GENERIC_VEC3 = 1 << 8,
		GENERIC_FLOAT = 1 << 9,

		INTERLACED_GOOCH = POSITIONS_VEC3 | NORMALS,
		INTERLACED_ALBEDO_ONLY = POSITIONS_VEC3 | UVS,
		INTERLACED_BLINN_PHONG = POSITIONS_VEC3 | NORMALS | UVS,
		INTERLACED_BLINN_PHONG_NORMALMAPPED = POSITIONS_VEC3 | NORMALS | TANGENTS | UVS
	};
	enum F_3dFileAttributes: unsigned short
	{
		INVALID = 0,

		POSITIONS = 1 << 0,
		NORMALS = 1 << 1,
		TANGENTS = 1 << 2,
		UVS = 1 << 3,

		INDICES = 1 << 4,
		ALBEDO_MAP = 1 << 5,
		SPECULAR_MAP = 1 << 6,
		NORMAL_MAP = 1 << 7,

		EVERYTHING = -1,
		GOOCH_ONLY = POSITIONS | NORMALS | INDICES,
		ALBEDO_ONLY = POSITIONS | UVS | INDICES | ALBEDO_MAP,
		BLINN_PHONG = POSITIONS | NORMALS | UVS | INDICES | ALBEDO_MAP | SPECULAR_MAP,
		BLINN_PHONG_NORMALMAPPED = POSITIONS | NORMALS | TANGENTS | UVS | INDICES | ALBEDO_MAP | SPECULAR_MAP | NORMAL_MAP
	};
}//!sge