#include <Scout/IInputSystem.h>

#include <stdexcept>

#include <Scout/SdlInput/Window.h>

namespace Scout
{
	std::unique_ptr<IWindow> MakeWindow(const WindowDef def)
	{
		return std::make_unique<Window_SDL_OpenGL33>(def.windowName, def.windowWidth, def.windowHeight);
	}
}