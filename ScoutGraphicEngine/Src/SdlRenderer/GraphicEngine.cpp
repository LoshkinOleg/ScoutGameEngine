#include <Scout/SdlRenderer/GraphicEngine.h>

#include <stdexcept>

#include <Scout/SdlInput/Window.h>

namespace Scout
{
	GraphicsEngine_SDL::GraphicsEngine_SDL(const std::uint32_t viewportWidth, const std::uint32_t viewportHeight) :
		viewportWidth_(viewportWidth), viewportHeight_(viewportHeight)
	{
		pRenderer_ = SDL_CreateRenderer(Window_SDL_OpenGL33::Get().GetSdlWindow(), -1, SDL_RENDERER_ACCELERATED);
	}

	void GraphicsEngine_SDL::SetViewMatrix(const Mat4x4& viewMatrix)
	{
		viewMatrix_ = viewMatrix;
	}

	ModelHandle GraphicsEngine_SDL::MakeModel(const ModelDef def)
	{ 
		if (!def.pModelMatrix) throw std::runtime_error("GraphicsEngine_SDL::MakeModel: def.pModelMatrix is null, but GraphicsEngine_SDL implementation expects the user to handle lifetime of the model matrix.");

		modelMatrices_.push_back(def.pModelMatrix);
		models_.emplace_back(def.vertices, def.modelColor);
		return models_.size() - 1;
	}

	GraphicsEngine_SDL::~GraphicsEngine_SDL()
	{
		if (pRenderer_) SDL_DestroyRenderer(pRenderer_);
	}

	void GraphicsEngine_SDL::Schedule(const ModelHandle modelId)
	{
		toRender_.insert(modelId);
	}

	void GraphicsEngine_SDL::Unschedule(const ModelHandle modelId)
	{
		toRender_.erase(modelId);
	}

	void GraphicsEngine_SDL::Update()
	{
		SDL_RenderPresent(pRenderer_);
		SDL_SetRenderDrawColor(pRenderer_, (Uint8)(CLEAR_COLOR_.r * 255.0f), (Uint8)(CLEAR_COLOR_.g * 255.0f), (Uint8)(CLEAR_COLOR_.b * 255.0f), (Uint8)(CLEAR_COLOR_.a * 255.0f));
		SDL_RenderClear(pRenderer_);

		for (const auto& handle : toRender_)
		{
			models_[handle].Draw(*this, viewMatrix_, projMatrix_, *modelMatrices_[handle]);
		}

		if (uiSystemRender_) uiSystemRender_();
	}

	void GraphicsEngine_SDL::DrawPoint(
		const float xPos, const float yPos,
		const float thickness,
		const Color color)
	{
		SDL_SetRenderDrawColor(pRenderer_, (Uint8)(color.r * 255.f), (Uint8)(color.g * 255.0f), (Uint8)(color.b * 255.0f), (Uint8)(color.a * 255.0f));
		SDL_RenderDrawPoint(pRenderer_, (int)xPos * viewportWidth_, (int)yPos * viewportHeight_);
	}

	void GraphicsEngine_SDL::DrawLine(
		const float xPos0, const float yPos0,
		const float xPos1, const float yPos1,
		const float thickness,
		const Color color)
	{
		SDL_SetRenderDrawColor(pRenderer_, (Uint8)(color.r * 255.f), (Uint8)(color.g * 255.0f), (Uint8)(color.b * 255.0f), (Uint8)(color.a * 255.0f));
		SDL_RenderDrawLine(
			pRenderer_,
			(int)(xPos0 * (float)viewportWidth_),
			(int)(yPos0 * (float)viewportHeight_),
			(int)(xPos1 * (float)viewportWidth_),
			(int)(yPos1 * (float)viewportHeight_));
	}

	void GraphicsEngine_SDL::DrawTriangle(
		const float xPos0, const float yPos0,
		const float xPos1, const float yPos1,
		const float xPos2, const float yPos2,
		const float thickness,
		const Color color,
		const bool filled)
	{
		if (filled) throw std::runtime_error("DrawTriangle: Implement usage of filled");

		DrawLine(xPos0, yPos0, xPos1, yPos1, thickness, color);
		DrawLine(xPos1, yPos1, xPos2, yPos2, thickness, color);
		DrawLine(xPos2, yPos2, xPos0, yPos0, thickness, color);
	}

	void GraphicsEngine_SDL::DrawCircle(
		const float xPos, const float yPos,
		const float radius,
		const float thickness,
		const Color color,
		const bool filled)
	{
		throw std::runtime_error("GraphicsEngine_SDL::DrawCircle: Implement this.");
	}
	void* GraphicsEngine_SDL::GetImplementationApi()
	{
		return pRenderer_;
	}
	void GraphicsEngine_SDL::RegisterUiDrawingCallback(ImmediateModeUiDrawingCallback callback)
	{
		uiSystemRender_ = callback;
	}
	SDL_Renderer* GraphicsEngine_SDL::GetSdlRenderer()
	{
		return pRenderer_;
	}
	std::uint32_t GraphicsEngine_SDL::GetViewportWidth() const
	{
		return viewportWidth_;
	}
	std::uint32_t GraphicsEngine_SDL::GetViewportHeight() const
	{
		return viewportHeight_;
	}
}