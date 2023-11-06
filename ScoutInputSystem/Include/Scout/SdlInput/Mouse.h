#pragma once

#include <SDL.h>
#ifdef main
#undef main
#endif

#include <Scout/IInputSystem.h>
#include <Scout/SdlInput/TypedefAndEnum.h>

namespace Scout
{
    // TODO: Handle release mouse btn events
    class Mouse_SDL final : public IMouse
    {
    public:
        void RegisterMouseMovementCallback(const MouseMovementCallback callback) override;
        void RegisterMouseScrollCallback(const MouseScrollCallback callback) override;
        void RegisterMouseBtnCallback(const MouseKey key, const MouseBtnCallback callback) override;
        bool IsKeyDown(const MouseKey key) override;
        std::pair<float, float> GetMousePos() override;
        std::pair<float, float> GetMouseDelta() override;
        std::pair<float, float> GetMouseWheelDelta() override;

        void Process(const SDL_Event& e, const std::uint32_t width, const std::uint32_t height);

    private:
        void UpdateMouseDeltas_(const SDL_Event& e, const std::uint32_t width, const std::uint32_t height);

    private:
        std::vector<MouseMovementCallback> movementCallbacks_{};
        std::vector<MouseScrollCallback> scrollwheelCallbacks_{};
        std::unordered_map<MouseKey_SDL, std::vector<MouseBtnCallback>> buttonsCallbacks_{};
        float lastUpdateMouseDelta_[2] = { 0.0f, 0.0f };
        float lastUpdateScrollwheelDelta_[2] = { 0.0f, 0.0f };
    };
}