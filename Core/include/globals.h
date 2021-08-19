#pragma once

#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace sge
{
	// Constexpr static consts. Don't take up space in global scope past compilation.
	constexpr const glm::ivec2 ZERO_IVEC2 = glm::ivec2(0, 0);
	constexpr const glm::ivec2 ONE_IVEC2 = glm::ivec2(1, 1);

	constexpr const glm::vec2 ZERO_VEC2 = glm::vec2(0.0f);
	constexpr const glm::vec2 ONE_VEC2 = glm::vec2(1.0f);
	constexpr const glm::vec2 EAST_VEC2 = glm::vec2(1.0f, 0.0f);
	constexpr const glm::vec2 WEST_VEC2 = glm::vec2(-1.0f, 0.0f);
	constexpr const glm::vec2 NORTH_VEC2 = glm::vec2(0.0f, 1.0f);
	constexpr const glm::vec2 SOUTH_VEC2 = glm::vec2(0.0f, -1.0f);

	constexpr const glm::vec3 ZERO_VEC3 = glm::vec3(0.0f);
	constexpr const glm::vec3 ONE_VEC3 = glm::vec3(1.0f);
	constexpr const glm::vec3 EAST_VEC3 = glm::vec3(1.0f, 0.0f, 0.0f);
	constexpr const glm::vec3 WEST_VEC3 = glm::vec3(-1.0f, 0.0f, 0.0f);
	constexpr const glm::vec3 NORTH_VEC3 = glm::vec3(0.0f, 1.0f, 0.0f);
	constexpr const glm::vec3 SOUTH_VEC3 = glm::vec3(0.0f, -1.0f, 0.0f);
	constexpr const glm::vec3 UP_VEC3 = glm::vec3(0.0f, 0.0f, 1.0f);
	constexpr const glm::vec3 DOWN_VEC3 = glm::vec3(0.0f, 0.0f, -1.0f);
	
	constexpr const glm::quat IDENTITY_QUAT = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	constexpr const glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
	constexpr const glm::vec3 WHITE = glm::vec3(1.0f);
	constexpr const glm::vec3 BLACK = glm::vec3(0.0f);
	constexpr const glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
	constexpr const glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
	constexpr const glm::vec3 DEFAULT_COLOR = glm::vec3(0.3f, 0.0f, 0.3f);
	constexpr const glm::vec3 INVERSE_DEFAULT_COLOR = glm::vec3(0.6f, 1.0f, 0.6f);

	constexpr const glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0f);
	constexpr const glm::mat4 DEFAULT_VIEW_MATRIX = IDENTITY_MAT4;

	constexpr const float PI = 3.14f;

	constexpr const std::array<float, 12> QUAD_2D = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,  1.0f };
	constexpr const std::array<float, 18> QUAD_3D = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, -1.0f,  1.0f, 0.0f };
	constexpr const std::array<float, 216> CUBE =
	{
		-1, -1,  1,		 0,  0,  1,
		 1, -1,  1,		 0,  0,  1,
		 1,  1,  1,		 0,  0,  1,
		-1, -1,  1,		 0,  0,  1,
		 1,  1,  1,		 0,  0,  1,
		-1,  1,  1,		 0,  0,  1,
		 1, -1,  1,		 1,  0,  0,
		 1, -1, -1,		 1,  0,  0,
		 1,  1, -1,		 1,  0,  0,
		 1, -1,  1,		 1,  0,  0,
		 1,  1, -1,		 1,  0,  0,
		 1,  1,  1,		 1,  0,  0,
		 1, -1, -1,		 0,  0, -1,
		-1, -1, -1,		 0,  0, -1,
		-1,  1, -1,		 0,  0, -1,
		 1, -1, -1,		 0,  0, -1,
		-1,  1, -1,		 0,  0, -1,
		 1,  1, -1,		 0,  0, -1,
		-1, -1, -1,		-1,  0,  0,
		-1, -1,  1,		-1,  0,  0,
		-1,  1,  1,		-1,  0,  0,
		-1, -1, -1,		-1,  0,  0,
		-1,  1,  1,		-1,  0,  0,
		-1,  1, -1,		-1,  0,  0,
		-1,  1,  1,		 0,  1,  0,
		 1,  1,  1,		 0,  1,  0,
		 1,  1, -1,		 0,  1,  0,
		-1,  1,  1,		 0,  1,  0,
		 1,  1, -1,		 0,  1,  0,
		-1,  1, -1,		 0,  1,  0,
		-1, -1, -1,		 0, -1,  0,
		 1, -1, -1,		 0, -1,  0,
		 1, -1,  1,		 0, -1,  0,
		-1, -1, -1,		 0, -1,  0,
		 1, -1,  1,		 0, -1,  0,
		-1, -1,  1,		 0, -1,  0
	};

	constexpr const glm::ivec2 RESOLUTION = glm::ivec2(1024, 768);
	constexpr const glm::ivec2 ASPECT = glm::ivec2(4, 3);
	constexpr const float HORIZONTAL_FULL_FOV = glm::radians(90.0f);
	constexpr const std::array<float, 2> FULL_FOV = { HORIZONTAL_FULL_FOV, HORIZONTAL_FULL_FOV * (float)ASPECT[1] / (float)ASPECT[0] };
	constexpr const float WINDOW_PROJECTION_NEAR = 0.01f;
	constexpr const float WINDOW_PROJECTION_FAR = 100.01f;

	// Global runtime consts.
	const glm::mat4 WINDOW_PROJECTION = glm::perspective(HORIZONTAL_FULL_FOV, (float)ASPECT[0] / (float)ASPECT[1], WINDOW_PROJECTION_NEAR, WINDOW_PROJECTION_FAR);
	const glm::mat4 WINDOW_PROJECTION_INVERSE = glm::inverse(WINDOW_PROJECTION);

	// Common enums.
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
		DEFERRED_PASS = 1 << 7,

		MAX_VALUE = DEFERRED_PASS
	};
	enum class ImageType: uint8_t
	{
		INVALID = 0,

		ALBEDO_MAP = 1 << 0,
		SPECULAR_MAP = 1 << 1,
		NORMAL_MAP = 1 << 2,
		OTHER = 1 << 3,

		MAX_VALUE = OTHER
	};
	enum class Primitive: uint32_t
	{
		POINTS = 0, // GL_POINTS
		LINES = 1, // GL_LINES
		LINE_STRIP = 3, // GL_LINE_STRIP
		TRIANGLES = 4, // GL_TRIANGLES

		MAX_VALUE = TRIANGLES
	};
}//!sge
