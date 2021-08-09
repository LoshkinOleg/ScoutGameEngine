#pragma once

#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glad/glad.h>

namespace sge
{
	// Constexpr static consts. Don't take up space in global scope past compilation.
	constexpr const glm::ivec2 ZERO_IVEC2 = glm::ivec2(0, 0);
	constexpr const glm::ivec2 ONE_IVEC2 = glm::ivec2(1, 1);

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

	constexpr const glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0f);
	constexpr const glm::mat4 DEFAULT_VIEW_MATRIX = IDENTITY_MAT4;

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

	constexpr const uint32_t HASHING_SEED = 0x1337;

	// Global runtime consts.
	const glm::mat4 WINDOW_PROJECTION = glm::perspective(HORIZONTAL_FULL_FOV, (float)ASPECT[0] / (float)ASPECT[1], WINDOW_PROJECTION_NEAR, WINDOW_PROJECTION_FAR);
	const glm::mat4 WINDOW_PROJECTION_INVERSE = glm::inverse(WINDOW_PROJECTION);
}//!sge
