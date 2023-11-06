#include <Scout/SdlInput/Keyboard.h>

#include <stdexcept>

#include <Scout/SdlInput/TypedefAndEnum.h>

namespace Scout
{
    void Keyboard_SDL::RegisterKeyboardCallback(const KeyboardKey key, const KeyboardCallback callback)
    {
        const auto sdlKey = ToImplEnum_SdlInput(key);
        callbacks_[sdlKey].push_back(callback);
    }
    bool Keyboard_SDL::IsKeyDown(const KeyboardKey key)
    {
        const auto* state = SDL_GetKeyboardState(NULL);
        const auto sdlKey = ToImplEnum_SdlInput(key);
        return state[(Sint32)sdlKey];
    }

    void Keyboard_SDL::Process(const SDL_Event& e)
    {
        if (e.type != SDL_KEYDOWN && e.type != SDL_KEYUP) throw std::runtime_error("Keyboard's Process() method called for an event that is not a keyboard event.");
        auto& callbacks = callbacks_[(KeyboardKey_SDL)e.key.keysym.scancode];
        for (auto& callback : callbacks)
        {
            callback(e.key.state, e.key.repeat);
        }
    }
}