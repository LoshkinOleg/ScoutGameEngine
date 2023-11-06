#pragma once

#include <stdexcept>
#include <set>

#include <SDL.h>
#ifdef main
#undef main
#endif

#include <Scout/IGraphicEngine.h>
#include <Scout/Math.h>
#include <Scout/SdlRenderer/Model.h>

// TODO: GraphicsEngine_SDL violates Liskov's substitution principle in it's current state: it does not support 3D mesh rendering

namespace Scout
{
    /*
        TODO:
        - implement filled shapes in immediate mode drawing
        - implement thickness of lines in immediate mode drawing
        - implement drawing of circles in immediate mode
    */
	class GraphicsEngine_SDL final: public IGraphicsEngine
	{
	public:
        GraphicsEngine_SDL() = delete;
        SCOUT_NO_COPY(GraphicsEngine_SDL);
        SCOUT_NO_MOVE(GraphicsEngine_SDL);
        ~GraphicsEngine_SDL();

        GraphicsEngine_SDL(const std::uint32_t viewportWidth, const std::uint32_t viewportHeight);

        void SetViewMatrix(const Mat4x4& viewMatrix) override;
        ModelHandle MakeModel(const ModelDef def) override;

        void Schedule(const ModelHandle modelId) override;
        void Unschedule(const ModelHandle modelId) override;
        void Update() override;
        void DrawPoint(
            const float xPos, const float yPos,
            const float thickness,
            const Color color) override;
        void DrawLine(
            const float xPos0, const float yPos0,
            const float xPos1, const float yPos1,
            const float thickness,
            const Color color) override;
        void DrawTriangle(
            const float xPos0, const float yPos0,
            const float xPos1, const float yPos1,
            const float xPos2, const float yPos2,
            const float thickness,
            const Color color,
            const bool filled) override;
        void DrawCircle(
            const float xPos, const float yPos,
            const float radius,
            const float thickness,
            const Color color,
            const bool filled) override;

        void* GetImplementationApi() override;
        void RegisterUiDrawingCallback(ImmediateModeUiDrawingCallback callback) override;

        SDL_Renderer* GetSdlRenderer();
        std::uint32_t GetViewportWidth() const;
        std::uint32_t GetViewportHeight() const;

    private:
        constexpr static const Color CLEAR_COLOR_ = COLOR_BLACK;

        SDL_Renderer* pRenderer_ = nullptr;
        std::uint32_t viewportWidth_ = 0, viewportHeight_ = 0;

        ImmediateModeUiDrawingCallback uiSystemRender_{};

        Mat4x4 viewMatrix_ = MAT4_IDENTITY;
        Mat4x4 projMatrix_ = UNIT_ORTHO_PROJECTION;
        std::vector<Model_SDL> models_{};
        std::vector<Mat4x4*> modelMatrices_{};
        std::set<ModelHandle> toRender_{};
	};
}