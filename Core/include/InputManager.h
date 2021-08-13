#pragma once

#include <array>

#include <SDL_keycode.h>
#include <SDL_mouse.h>

#include "globals.h"

union SDL_Event;

namespace sge
{
	struct ControlScheme
	{

	};

	struct Command
	{
	public:
		enum Type : uint32_t
		{
			INVALID = 0,
			EAST = SDLK_d,
			WEST = SDLK_a,
			NORTH = SDLK_w,
			SOUTH = SDLK_s,
			UP = SDLK_SPACE,
			DOWN = SDLK_LCTRL,
			CONFIRM = SDLK_RETURN,
			CANCEL = SDLK_ESCAPE,
			MODIFIER = SDLK_LSHIFT,
			LMB = SDL_BUTTON_LEFT,
			MMB = SDL_BUTTON_MIDDLE,
			RMB = SDL_BUTTON_RIGHT
		};

		Type type = Type::INVALID;
		bool pressedDown = false;
	};
	struct MouseMotion
	{
		glm::ivec2 lastPos = -ONE_IVEC2; // < 0 means invalid since screen pos range is [0;RESOLUTION[
		glm::ivec2 currentPos = -ONE_IVEC2;

		glm::ivec2 GetDeltaScreenSpace() const;
		glm::vec2 GetPosNdc() const;
		glm::vec2 GetDeltaNdc() const;
		glm::vec3 GetPosWorldSpace(const glm::mat4& view) const;
	};

	class InputManager
	{
	public:
		void ClearQueues();
		void StoreInput(const SDL_Event& event);

	private:
		constexpr static const uint32_t COMMAND_QUEUE_SIZE_ = 64;
		constexpr static const uint32_t MOUSE_MOVEMENT_QUEUE_SIZE_ = 32;

		std::array<Command, COMMAND_QUEUE_SIZE_> commandQueue_ = { Command() };
		uint32_t lastCommand_ = 0;
		std::array<MouseMotion, MOUSE_MOVEMENT_QUEUE_SIZE_> mouseMotionQueue_ = { MouseMotion() };
		uint32_t lastMouseMotion_ = 0;
	};
}//!sge