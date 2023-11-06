#include <Scout/SdlInput/Mouse.h>

#include <stdexcept>

#include <Scout/SdlInput/TypedefAndEnum.h>
#include <Scout/SdlInput/Window.h>

namespace Scout
{
	void Mouse_SDL::RegisterMouseMovementCallback(const MouseMovementCallback callback)
	{
		movementCallbacks_.push_back(callback);
	}
	void Mouse_SDL::RegisterMouseScrollCallback(const MouseScrollCallback callback)
	{
		scrollwheelCallbacks_.push_back(callback);
	}
	void Mouse_SDL::RegisterMouseBtnCallback(const MouseKey key, const MouseBtnCallback callback)
	{
		const auto sdlKey = ToImplEnum_SdlInput(key);
		buttonsCallbacks_[sdlKey].push_back(callback);
	}
	bool Mouse_SDL::IsKeyDown(const MouseKey key)
	{
		const auto buttonsMask = SDL_GetMouseState(NULL, NULL);
		const auto sdlKey = ToImplEnum_SdlInput(key);
		return buttonsMask & (Uint32)sdlKey;
	}
	std::pair<float, float> Mouse_SDL::GetMousePos()
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		const float width = (float)Window_SDL_OpenGL33::Get().GetWidth();
		const float height = (float)Window_SDL_OpenGL33::Get().GetHeight();
		return { (float)x / width, (float)y / height };
	}
	std::pair<float, float> Mouse_SDL::GetMouseDelta()
	{
		return { lastUpdateMouseDelta_[0], lastUpdateMouseDelta_[1] };
	}
	std::pair<float, float> Mouse_SDL::GetMouseWheelDelta()
	{
		return { lastUpdateScrollwheelDelta_[0], lastUpdateScrollwheelDelta_[1] };
	}

	void Mouse_SDL::Process(const SDL_Event& e, const std::uint32_t width, const std::uint32_t height)
	{
		if (e.type != SDL_MOUSEMOTION &&
			e.type != SDL_MOUSEWHEEL &&
			e.type != SDL_MOUSEBUTTONDOWN &&
			e.type != SDL_MOUSEBUTTONUP) throw std::runtime_error("Mouse's Process() method called with an event that is not a mouse event.");

		const float x = (float)e.button.x;
		const float y = (float)e.button.y;

		if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL)
		{
			UpdateMouseDeltas_(e, width, height);
			if (e.type == SDL_MOUSEMOTION)
			{
				for (auto& callback : movementCallbacks_)
				{
					callback(x / (float)width, y / (float)height);
				}
			}
			else
			{
				for (auto& callback : scrollwheelCallbacks_)
				{
					callback((float)e.wheel.y, (float)e.wheel.x);
				}
			}
		}
		else if (e.type != SDL_MOUSEBUTTONDOWN)
		{
			auto& callbacks = buttonsCallbacks_[(MouseKey_SDL)SDL_BUTTON(e.button.button)];
			for (auto& callback : callbacks)
			{
				callback(e.button.state, e.button.clicks - 1, x / (float)width, y / (float)height);
			}
		}
	}

	void Mouse_SDL::UpdateMouseDeltas_(const SDL_Event& e, const std::uint32_t width, const std::uint32_t height)
	{
		if (e.type == SDL_MOUSEMOTION)
		{
			// TODO: make sure SDL's screenspace coordinate system has +Y going up to down.
			const float deltaX = (float)e.motion.x;
			const float deltaY = (float)e.motion.y;
			lastUpdateMouseDelta_[0] = deltaX / (float)width;
			lastUpdateMouseDelta_[1] = deltaY / (float)height;
		}
		else
		{
			lastUpdateScrollwheelDelta_[0] = (float)e.wheel.y;
			lastUpdateScrollwheelDelta_[1] = (float)e.wheel.x;
		}
	}
}