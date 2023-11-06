#pragma once

#include <SDL.h>
#ifdef main
#undef main
#endif

#include <Scout/IInputSystem.h>
#include <Scout/TypedefAndEnum.h>
#include <Scout/SdlInput/TypedefAndEnum.h>

namespace Scout
{
	class Keyboard_SDL final : public IKeyboard
	{
	public:
		void RegisterKeyboardCallback(const KeyboardKey key, const KeyboardCallback callback) override;
		bool IsKeyDown(const KeyboardKey key) override;

		void Process(const SDL_Event& e);

	private:
		std::unordered_map<KeyboardKey_SDL, std::vector<KeyboardCallback>> callbacks_{};
	};
}