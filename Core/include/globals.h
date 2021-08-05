#pragma once

#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace sge
{
	// Constexpr static consts. Don't take up space in global scope past compilation.
	constexpr const glm::ivec2 ZERO_IVEC2 = glm::ivec2(0, 0);
	constexpr const glm::ivec2 ONE_IVEC2 = glm::ivec2(1, 1);

	constexpr const std::array<float, 12> QUAD_2D_FULL = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,  1.0f };
	constexpr const std::array<float, 18> QUAD_3D_FULL = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, -1.0f,  1.0f, 0.0f };
	constexpr const std::array<float, 8> QUAD_2D_INDEXED = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f, -1.0f,  1.0f };
	constexpr const std::array<float, 12> QUAD_3D_INDEXED = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, -1.0f, 1.0f, 0.0f };
	constexpr const std::array<uint32_t, 6> QUAD_INDICES = { 0, 1, 2, 0, 2, 3 };

	constexpr const glm::ivec2 RESOLUTION = glm::ivec2(1024, 768);
	constexpr const glm::ivec2 ASPECT = glm::ivec2(4, 3);
	constexpr const float HORIZONTAL_FULL_FOV = glm::radians(90.0f);
	constexpr const std::array<float, 2> FULL_FOV = { HORIZONTAL_FULL_FOV, HORIZONTAL_FULL_FOV * (float)ASPECT[1] / (float)ASPECT[0] };
	constexpr const float WINDOW_PROJECTION_NEAR = 0.01f;
	constexpr const float WINDOW_PROJECTION_FAR = 1.01f;

	constexpr const uint32_t HASHING_SEED = 0x1337;

	// Global runtime consts.
	const glm::mat4 WINDOW_PROJECTION = glm::perspective(HORIZONTAL_FULL_FOV, (float)ASPECT[0] / (float)ASPECT[1], WINDOW_PROJECTION_NEAR, WINDOW_PROJECTION_FAR);
	const glm::mat4 WINDOW_PROJECTION_INVERSE = glm::inverse(WINDOW_PROJECTION);
}//!sge
