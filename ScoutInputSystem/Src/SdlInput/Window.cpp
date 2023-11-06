#include <Scout/SdlInput/Window.h>



#include <stdexcept>

namespace Scout
{
	Window_SDL_OpenGL33* Window_SDL_OpenGL33::self_ = nullptr;

	Window_SDL_OpenGL33::Window_SDL_OpenGL33(const std::string_view windowName, const std::uint64_t width, const std::uint64_t height)
	{
		self_ = this;

		if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
		{
			throw std::runtime_error("SDL failed to initialize.");
		}

		Uint32 flags = SDL_WINDOW_SHOWN;

		// TODO: figure out why this is not enabled
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetSwapInterval(1); // Turn on V-sync
		flags |= SDL_WINDOW_OPENGL;

		pWindow_ = SDL_CreateWindow(
			windowName.data(),
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			(int)width, (int)height,
			flags);
		if (!pWindow_) throw std::runtime_error("SDL failed to create a window.");
	}
	Window_SDL_OpenGL33::~Window_SDL_OpenGL33()
	{
		if (pWindow_) SDL_DestroyWindow(pWindow_);
		SDL_Quit();
	}

	void Window_SDL_OpenGL33::PollEvents(const HidTypeFlag hid, bool& quit)
	{
		quit = false;

		while (SDL_PollEvent(&e))
		{
			bool consumed = false;
			if (uiCallback_)
			{
				try
				{
					consumed = uiCallback_(static_cast<const SDL_Event*>(&e));
				}
				catch (const std::exception&)
				{
					throw std::runtime_error("Window_SDL_OpenGL33::PollEvents: Failed to call imgui's input callback.");
				}
			}
			if (consumed) continue;

			if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
			{
				keyboard_.Process(e);
			}
			else if (
				e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL ||
				e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
			{
				mouse_.Process(e, width_, height_);
			}
			else if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}

	Window_SDL_OpenGL33& Window_SDL_OpenGL33::Get()
	{
		return *self_;
	}

	void Window_SDL_OpenGL33::RegisterKeyboardCallback(const KeyboardKey key, const KeyboardCallback callback)
	{
		keyboard_.RegisterKeyboardCallback(key, callback);
	}

	bool Window_SDL_OpenGL33::IsKeyDown(const KeyboardKey key)
	{
		return keyboard_.IsKeyDown(key);
	}

	void Window_SDL_OpenGL33::RegisterMouseMovementCallback(const MouseMovementCallback callback)
	{
		mouse_.RegisterMouseMovementCallback(callback);
	}

	void Window_SDL_OpenGL33::RegisterMouseScrollCallback(const MouseScrollCallback callback)
	{
		mouse_.RegisterMouseScrollCallback(callback);
	}

	void Window_SDL_OpenGL33::RegisterMouseBtnCallback(const MouseKey key, const MouseBtnCallback callback)
	{
		mouse_.RegisterMouseBtnCallback(key, callback);
	}

	bool Window_SDL_OpenGL33::IsKeyDown(const MouseKey key)
	{
		return mouse_.IsKeyDown(key);
	}

	std::pair<float, float> Window_SDL_OpenGL33::GetMousePos()
	{
		return mouse_.GetMousePos();
	}

	std::pair<float, float> Window_SDL_OpenGL33::GetMouseDelta()
	{
		return mouse_.GetMouseDelta();
	}

	std::pair<float, float> Window_SDL_OpenGL33::GetMouseWheelDelta()
	{
		return mouse_.GetMouseWheelDelta();
	}
	void* Window_SDL_OpenGL33::GetImplementationApi()
	{
		return pWindow_;
	}
	void Window_SDL_OpenGL33::RegisterUiProcessEventCallback(ImmediateModeUiProcessEventCallback callback)
	{
		uiCallback_ = callback;
	}
}