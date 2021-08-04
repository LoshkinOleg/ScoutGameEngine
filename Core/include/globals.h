#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL_keycode.h>
#include <SDL_mouse.h>

namespace sge
{
	// Constexpr static consts. Don't take up space in global scope past compilation.
	constexpr const glm::ivec2 RESOLUTION = glm::ivec2(1024, 768);
	constexpr const glm::ivec2 ASPECT = glm::ivec2(4, 3);
	constexpr const glm::ivec2 ZERO_IVEC2 = glm::ivec2(0, 0);
	constexpr const glm::ivec2 ONE_IVEC2 = glm::ivec2(1, 1);
	constexpr const float HORIZONTAL_FULL_FOV = glm::radians(90.0f);
	constexpr const float FULL_FOV[2] = { HORIZONTAL_FULL_FOV, HORIZONTAL_FULL_FOV * (float)ASPECT[1] / (float)ASPECT[0] };
	constexpr const float WINDOW_PROJECTION_NEAR = 0.01f;
	constexpr const float WINDOW_PROJECTION_FAR = 1.01f;
	constexpr const uint32_t HASHING_SEED = 0x1337;

	// Global variables.
	const glm::mat4 WINDOW_PROJECTION = glm::perspective(HORIZONTAL_FULL_FOV, (float)ASPECT[0] / (float)ASPECT[1], WINDOW_PROJECTION_NEAR, WINDOW_PROJECTION_FAR);
	const glm::mat4 WINDOW_PROJECTION_INVERSE = glm::inverse(WINDOW_PROJECTION);
}//!sge
