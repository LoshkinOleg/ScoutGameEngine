#pragma once

#include <cstdint>

namespace sge
{
	enum class Mutability: uint32_t
	{
		INVALID = 0,

		STATIC = 0x88E4, // GL_STATIC_DRAW
		DYNAMIC = 0x88E8 // GL_DYNAMIC_DRAW
	};
	enum class NumberType: uint32_t
	{
		INVALID = 0,

		UINT = 0x1405, // GL_UNSIGNED_INT
		FLOAT = 0x1406 // GL_FLOAT
	};
	enum class ShadingMode: uint8_t
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
	enum class TextureType: uint8_t
	{
		INVALID = 0,

		ALBEDO_MAP = 1 << 0,
		SPECULAR_MAP = 1 << 1,
		NORMAL_MAP = 1 << 2
	};
	enum class DrawingPrimitive: uint32_t
	{
		POINTS = 0, // GL_POINTS
		LINES = 1, // GL_LINES
		LINE_STRIP = 3, // GL_LINE_STRIP
		TRIANGLES = 4, // GL_TRIANGLES
	};
	enum class VertexBufferTarget: uint32_t
	{
		INVALID = 0,

		VBO = 0x8892, // GL_ARRAY_BUFFER
		EBO = 0x8893 // GL_ELEMENT_ARRAY_BUFFER
	};
	enum class VertexBufferType: uint16_t
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
		GENERIC_FLOAT = 1 << 9
	};
	enum class GltfAttributes: uint8_t
	{
		INVALID = 0,

		POSITIONS = 1 << 0,
		NORMALS = 1 << 1,
		TANGENTS = 1 << 2,
		UVS = 1 << 3,

		INDICES = 1 << 4,

		EVERYTHING = POSITIONS | NORMALS | TANGENTS | UVS | INDICES
	};
}//!sge