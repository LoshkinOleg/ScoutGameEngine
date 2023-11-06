#pragma once

#include <SDL.h>
#ifdef main
#undef main
#endif

#include <Scout/IInputSystem.h>
#include <Scout/SdlInput/Keyboard.h>
#include <Scout/SdlInput/Mouse.h>

namespace Scout
{
    class Window_SDL_OpenGL33 final : public IWindow
    {
    public:
        SCOUT_NO_COPY(Window_SDL_OpenGL33);
        SCOUT_NO_MOVE(Window_SDL_OpenGL33);
        Window_SDL_OpenGL33() = delete;

        Window_SDL_OpenGL33(
            const std::string_view windowName,
            const std::uint64_t width, const std::uint64_t height);
        ~Window_SDL_OpenGL33();

        void PollEvents(const HidTypeFlag hid, bool& quit) override;

        void Update() override
        {}

        // TODO: move to IWindow?
        static void* GetGraphicalApiFunction(const char* proc)
        {
            return SDL_GL_GetProcAddress(proc);
        }

        inline std::uint32_t GetWidth() const
        {
            return width_;
        }
        inline std::uint32_t GetHeight() const
        {
            return height_;
        }
        inline SDL_Window* const GetSdlWindow() const
        {
            return pWindow_;
        }

        void RegisterKeyboardCallback(const KeyboardKey key, const KeyboardCallback callback) override;
        bool IsKeyDown(const KeyboardKey key) override;
        void RegisterMouseMovementCallback(const MouseMovementCallback callback) override;
        void RegisterMouseScrollCallback(const MouseScrollCallback callback) override;
        void RegisterMouseBtnCallback(const MouseKey key, const MouseBtnCallback callback) override;
        bool IsKeyDown(const MouseKey key) override;
        std::pair<float, float> GetMousePos() override;
        std::pair<float, float> GetMouseDelta() override;
        std::pair<float, float> GetMouseWheelDelta() override;

        void* GetImplementationApi() override;
        void RegisterUiProcessEventCallback(ImmediateModeUiProcessEventCallback callback) override;

        static Window_SDL_OpenGL33& Get(); // TODO: get rid of this sheit

    private:
        Keyboard_SDL keyboard_; // TODO: these should be pointers to abstractions?...
        Mouse_SDL mouse_;

        std::uint32_t width_, height_;

        SDL_Event e{};
        SDL_Window* pWindow_ = nullptr;

        ImmediateModeUiProcessEventCallback uiCallback_{};

        static Window_SDL_OpenGL33* self_; // TODO: get rid of this sheit
    };
}