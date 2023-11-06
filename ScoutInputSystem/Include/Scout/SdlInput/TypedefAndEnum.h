#pragma once

#include <cstdint>
#include <stdexcept>

#include <SDL.h>
#ifdef main
#undef main
#endif

#include <Scout/TypedefAndEnum.h>

namespace Scout
{
    enum class MouseKey_SDL : Uint32
    {
        NONE = 0,

        LMB = SDL_BUTTON_LMASK,
        RMB = SDL_BUTTON_RMASK,
        MMB = SDL_BUTTON_MMASK,
        M4 = SDL_BUTTON_X1MASK, // TODO: check this mapping is correct
        M5 = SDL_BUTTON_X2MASK,
    };

    enum class KeyboardKey_SDL : Sint32
    {
        NONE = SDL_FIRSTEVENT,

        ESCAPE = SDL_SCANCODE_ESCAPE,

        W = SDL_SCANCODE_W,
        UP_ARROW = SDL_SCANCODE_UP,
        A = SDL_SCANCODE_A,
        LEFT_ARROW = SDL_SCANCODE_LEFT,
        S = SDL_SCANCODE_S,
        DOWN_ARROW = SDL_SCANCODE_DOWN,
        D = SDL_SCANCODE_D,
        RIGHT_ARROW = SDL_SCANCODE_RIGHT,

        E = SDL_SCANCODE_E,
        F = SDL_SCANCODE_F,

        Q = SDL_SCANCODE_Q,
        R = SDL_SCANCODE_R,
        C = SDL_SCANCODE_C,
        X = SDL_SCANCODE_X,
        Z = SDL_SCANCODE_Z,
        ONE = SDL_SCANCODE_1,
        TWO = SDL_SCANCODE_2,
        THREE = SDL_SCANCODE_3,
        FOUR = SDL_SCANCODE_4,
        TILDE = SDL_SCANCODE_GRAVE,
        SPACE = SDL_SCANCODE_SPACE,
        TAB = SDL_SCANCODE_TAB,

        LCTRL = SDL_SCANCODE_LCTRL,
        LSHIFT = SDL_SCANCODE_LSHIFT,
        LALT = SDL_SCANCODE_LALT,
    };

    inline MouseKey_SDL ToImplEnum_SdlInput(const MouseKey key)
    {
        switch (key)
        {

            case MouseKey::NONE: return MouseKey_SDL::NONE;
            case MouseKey::LMB: return MouseKey_SDL::LMB;
            case MouseKey::RMB: return MouseKey_SDL::RMB;
            case MouseKey::MMB: return MouseKey_SDL::MMB;
            case MouseKey::M4: return MouseKey_SDL::M4;
            case MouseKey::M5: return MouseKey_SDL::M5;
            
            default:
            {
                throw std::runtime_error("MouseKey_SDL ToImplEnum_SdlInput(const MouseKey key): No conversion from specified MouseKey to MouseKey_SDL.");
            }break;

        }
    }
    inline KeyboardKey_SDL ToImplEnum_SdlInput(const KeyboardKey key)
    {
        switch (key)
        {

            case KeyboardKey::NONE: return KeyboardKey_SDL::NONE;

            case KeyboardKey::ESCAPE: return KeyboardKey_SDL::ESCAPE;

            case KeyboardKey::W: return KeyboardKey_SDL::W;
            case KeyboardKey::UP_ARROW: return KeyboardKey_SDL::UP_ARROW;
            case KeyboardKey::A: return KeyboardKey_SDL::A;
            case KeyboardKey::LEFT_ARROW: return KeyboardKey_SDL::LEFT_ARROW;
            case KeyboardKey::S: return KeyboardKey_SDL::S;
            case KeyboardKey::DOWN_ARROW: return KeyboardKey_SDL::DOWN_ARROW;
            case KeyboardKey::D: return KeyboardKey_SDL::D;
            case KeyboardKey::RIGHT_ARROW: return KeyboardKey_SDL::RIGHT_ARROW;

            case KeyboardKey::E: return KeyboardKey_SDL::E;
            case KeyboardKey::F: return KeyboardKey_SDL::F;

            case KeyboardKey::Q: return KeyboardKey_SDL::Q;
            case KeyboardKey::R: return KeyboardKey_SDL::R;
            case KeyboardKey::C: return KeyboardKey_SDL::C;
            case KeyboardKey::X: return KeyboardKey_SDL::X;
            case KeyboardKey::Z: return KeyboardKey_SDL::Z;
            case KeyboardKey::ONE: return KeyboardKey_SDL::ONE;
            case KeyboardKey::TWO: return KeyboardKey_SDL::TWO;
            case KeyboardKey::THREE: return KeyboardKey_SDL::THREE;
            case KeyboardKey::FOUR: return KeyboardKey_SDL::FOUR;
            case KeyboardKey::TILDE: return KeyboardKey_SDL::TILDE;
            case KeyboardKey::SPACE: return KeyboardKey_SDL::SPACE;
            case KeyboardKey::TAB: return KeyboardKey_SDL::TAB;

            case KeyboardKey::LCTRL: return KeyboardKey_SDL::LCTRL;
            case KeyboardKey::LSHIFT: return KeyboardKey_SDL::LSHIFT;
            case KeyboardKey::LALT: return KeyboardKey_SDL::LALT;
            
            default:
            {
                throw std::runtime_error("KeyboardKey_SDL ToImplEnum_SdlInput(const KeyboardKey key): No conversion from specified KeyboardKey to KeyboardKey_SDL.");
            }break;

        }
    }
}