#pragma once

#include <cstdint>
#include <memory>

#include <Scout/TypedefAndEnum.h>
#include <Scout/Math.h>
#include <Scout/Color.h>

namespace Scout
{
    struct ModelDef
    {
        std::vector<Vec3> vertices{};
        Mat4x4* pModelMatrix = nullptr;
        Color modelColor = COLOR_CLEAR;
    };

    class IGraphicsEngine
    {
    public:
        virtual void SetViewMatrix(const Mat4x4& viewMatrix) = 0;
        virtual ModelHandle MakeModel(const ModelDef def) = 0;

        virtual void Schedule(const ModelHandle modelId) = 0;
        virtual void Unschedule(const ModelHandle modelId) = 0;
        /*
        Call to draw scheduled elements on viewport.
        */
        virtual void Update() = 0;
        virtual void DrawPoint(
            const float xPos, const float yPos,
            const float thickness,
            const Color color) = 0;
        virtual void DrawLine(
            const float xPos0, const float yPos0,
            const float xPos1, const float yPos1,
            const float thickness,
            const Color color) = 0;
        virtual void DrawTriangle(
            const float xPos0, const float yPos0,
            const float xPos1, const float yPos1,
            const float xPos2, const float yPos2,
            const float thickness,
            const Color color,
            const bool filled) = 0;
        virtual void DrawCircle(
            const float xPos, const float yPos,
            const float radius,
            const float thickness,
            const Color color,
            const bool filled) = 0;

        virtual void* GetImplementationApi() = 0;
        virtual void RegisterUiDrawingCallback(ImmediateModeUiDrawingCallback callback) = 0;
    };

    /*
        TODO:
        - find way to enforce correct matching between shaderTypes, vertexShaders and fragmentShaders
    */
    struct GraphicsEngineDef
    {
        GraphicalApi implementation = GraphicalApi::NONE;
        void* graphicalApiProc = nullptr;

        std::uint32_t viewportWidth = 0;
        std::uint32_t viewportHeight = 0;

        bool enableDepthTest = true;

        bool enableFaceCulling = true;
        bool cullBackface = true;
        bool frontfaceIsCCW = true;

        bool enableAlphaBlending = true;
        const AlphaBlending sFactor = AlphaBlending::ALPHA;
        const AlphaBlending dFactor = AlphaBlending::ONE_MINUS_ALPHA;

        std::vector<ShadingType> shaderTypes{};
        std::vector<std::string> vertexShaders{};
        std::vector<std::string> fragmentShaders{};
    };

    /*
        Factory function for creating graphical renderer implementations.
    */
    std::unique_ptr<IGraphicsEngine> MakeGraphicsEngine(const GraphicsEngineDef def);
}
