#include <Scout/IGraphicEngine.h>

#include <stdexcept>

#include <Scout/SdlRenderer/GraphicEngine.h>

namespace Scout
{
    std::unique_ptr<IGraphicsEngine> MakeGraphicsEngine(const GraphicsEngineDef def)
    {
        return std::make_unique<GraphicsEngine_SDL>(def.viewportWidth, def.viewportHeight);
    }
}