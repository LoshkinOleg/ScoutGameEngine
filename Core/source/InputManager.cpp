#include "InputManager.h"

#include <SDL2/SDL.h>

#include "utility.h"

namespace sge
{
	void InputManager::ClearQueues()
	{
		commandQueue_ = { Command() };
		mouseMotionQueue_ = { MouseMotion() };
		lastCommand_ = 0;
		lastMouseMotion_ = 0;
	}
	void InputManager::StoreInput(const SDL_Event& event)
	{
		assert(lastCommand_ < COMMAND_QUEUE_SIZE_&& lastMouseMotion_ < MOUSE_MOVEMENT_QUEUE_SIZE_);

		switch (event.type)
		{
			case SDL_KEYDOWN:
				{
					switch (event.key.keysym.sym)
					{
						case Command::Type::EAST:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::EAST;
								cmd.pressedDown = true;
							}break;
						case Command::Type::NORTH:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::NORTH;
								cmd.pressedDown = true;
							}break;
						case Command::Type::WEST:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::WEST;
								cmd.pressedDown = true;
							}break;
						case Command::Type::SOUTH:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::SOUTH;
								cmd.pressedDown = true;
							}break;

						default:
							break;
					}
				}break;

			case SDL_KEYUP:
				{
					switch (event.key.keysym.sym)
					{
						case Command::Type::EAST:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::EAST;
							}break;
						case Command::Type::NORTH:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::NORTH;
							}break;
						case Command::Type::WEST:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::WEST;
							}break;
						case Command::Type::SOUTH:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::SOUTH;
							}break;

						default:
							break;
					}
				}break;

			case SDL_MOUSEMOTION:
				{
					auto& motion = mouseMotionQueue_[lastMouseMotion_++];
					motion.currentPos.x = event.motion.x;
					motion.currentPos.y = event.motion.y;
					motion.lastPos.x = motion.currentPos.x - event.motion.xrel;
					motion.lastPos.y = motion.currentPos.y - event.motion.yrel;
					assert(motion.lastPos.x >= 0 && motion.lastPos.y >= 0);
				}break;

			case SDL_MOUSEBUTTONDOWN:
				{
					switch (event.button.button)
					{
						case Command::Type::LMB:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::LMB;
								cmd.pressedDown = true;
							}break;
						case Command::Type::RMB:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::RMB;
								cmd.pressedDown = true;
							}break;
						default:
							break;
					}
				}break;

			case SDL_MOUSEBUTTONUP:
				{
					switch (event.button.button)
					{
						case Command::Type::LMB:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::LMB;
							}break;
						case Command::Type::RMB:
							{
								auto& cmd = commandQueue_[lastCommand_++];
								cmd.type = Command::Type::RMB;
							}break;
						default:
							break;
					}
				}break;
			default:
				break;
		}
	}

	glm::ivec2 MouseMotion::GetDeltaScreenSpace() const
	{
		return currentPos - lastPos;
	}
	glm::vec2 MouseMotion::GetPosNdc() const
	{
		return glm::vec2
		(
			RemapToRange<float>(0.0f, (float)RESOLUTION[0], -1.0f, 1.0f, (float)currentPos.x),
			RemapToRange<float>(0.0f, (float)RESOLUTION[1], 1.0f, -1.0f, (float)currentPos.y)
		);
	}
	glm::vec2 MouseMotion::GetDeltaNdc() const
	{
		const glm::vec2 ndcLastPos = glm::vec2
		(
			RemapToRange<float>(0.0f, (float)RESOLUTION[0], -1.0f, 1.0f, (float)lastPos.x),
			RemapToRange<float>(0.0f, (float)RESOLUTION[1], 1.0f, -1.0f, (float)lastPos.y)
		);
		const glm::vec2 ndcCurrentPos = GetPosNdc();
		return ndcCurrentPos - ndcLastPos;
	}
	glm::vec3 MouseMotion::GetPosWorldSpace(const glm::mat4& view) const
	{
		const glm::mat4 inverseView = glm::inverse(view);
		const glm::vec2 ndcPos = GetPosNdc();
		const glm::vec4 result = inverseView * WINDOW_PROJECTION_INVERSE * glm::vec4(ndcPos.x, ndcPos.y, 0.0f, 1.0f);
		return (glm::vec3)result;
	}
}//!sge