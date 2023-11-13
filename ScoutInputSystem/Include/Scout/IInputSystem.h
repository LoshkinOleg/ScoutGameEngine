#pragma once

#include <functional>
#include <tuple>
#include <memory>
#include <string>

#include <Scout/TypedefAndEnum.h>

namespace Scout
{
    class IWindow
    {
    public:
        /*
        Call to process window inputs.
        */
        virtual void PollEvents(const HidTypeFlag hid, bool& quit) = 0;
        /*
        Call to update window surface.
        */
        virtual void Update() = 0;

        virtual void RegisterKeyboardCallback(const KeyboardKey key, const KeyboardCallback callback) = 0;
        virtual bool IsKeyDown(const KeyboardKey key) = 0;

        virtual void RegisterMouseMovementCallback(const MouseMovementCallback callback) = 0;
        virtual void RegisterMouseScrollCallback(const MouseScrollCallback callback) = 0;
        virtual void RegisterMouseBtnCallback(const MouseKey key, const MouseBtnCallback callback) = 0;
        virtual bool IsKeyDown(const MouseKey key) = 0;
        /*
        @return Screenspace mouse coordinates. X is left to right, Y is top to bottom.
        */
        virtual std::pair<float, float> GetMousePos() = 0;
        /*
        TODO
        @return Screenspace mouse position delta. X is left to right, Y is top to bottom.
        */
        virtual std::pair<float, float> GetMouseDelta() = 0;
        virtual std::pair<float, float> GetMouseWheelDelta() = 0;

        virtual void* GetImplementationApi() = 0;
        virtual void RegisterUiProcessEventCallback(ImmediateModeUiProcessEventCallback callback) = 0;
    };

    class IKeyboard
    {
    public:
        virtual void RegisterKeyboardCallback(const KeyboardKey key, const KeyboardCallback callback) = 0;
        virtual bool IsKeyDown(const KeyboardKey key) = 0;
    };

    class IMouse
    {
    public:
        virtual void RegisterMouseMovementCallback(const MouseMovementCallback callback) = 0;
        virtual void RegisterMouseScrollCallback(const MouseScrollCallback callback) = 0;
        virtual void RegisterMouseBtnCallback(const MouseKey key, const MouseBtnCallback callback) = 0;
        virtual bool IsKeyDown(const MouseKey key) = 0;
        /*
        @return Screenspace mouse coordinates. X is left to right, Y is top to bottom.
        */
        virtual std::pair<float, float> GetMousePos() = 0;
        /*
        TODO
        @return Screenspace mouse position delta. X is left to right, Y is top to bottom.
        */
        virtual std::pair<float, float> GetMouseDelta() = 0;
        virtual std::pair<float, float> GetMouseWheelDelta() = 0;
    };

    struct WindowDef
    {
        InputApi implementation = InputApi::NONE;
        std::string windowName = "";
        std::uint32_t windowWidth = 0, windowHeight = 0;
    };

    std::unique_ptr<IWindow> MakeWindow(const WindowDef def);
}
