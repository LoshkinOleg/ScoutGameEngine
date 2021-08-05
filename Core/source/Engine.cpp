#include "Engine.h"

#include <chrono>
#include <iostream>

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "globals.h"

namespace sge
{
	constexpr static const uint32_t SDL_INIT_FLAGS_ = SDL_INIT_VIDEO /* | SDL_INIT_AUDIO */;
	constexpr static const int32_t SDL_PROFILE_ = SDL_GL_CONTEXT_PROFILE_CORE;
	constexpr static const int32_t GL_VERSION_[2] = { 4,5 };
	constexpr static const uint32_t CREATE_WINDOW_FLAGS_ = SDL_WINDOW_OPENGL;
	constexpr static const bool VSYNC_ = true;

	void Engine::Init()
	{
		SDL_Init(SDL_INIT_FLAGS_);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_PROFILE_);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_VERSION_[0]);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_VERSION_[1]);
		// SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Note: SDL doc recommends not using it if you don't know what you're doing.
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); // Note: is this really necessary? Isn't it enabled by default?
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		window_ = SDL_CreateWindow(
			"ScoutGameEngine",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			RESOLUTION[0],
			RESOLUTION[1],
			CREATE_WINDOW_FLAGS_);
		assert(window_);

		glContextPtr_ = SDL_GL_CreateContext(window_);
		SDL_GL_MakeCurrent(window_, glContextPtr_);
		SDL_GL_SetSwapInterval((int32_t)VSYNC_);
		const auto success = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
		assert(success);

		renderer_.Init();
		app_->Init();
		resourceManager_.PostInit();
	}
	void Engine::Shutdown()
	{
		app_->Shutdown();
		renderer_.Shutdown();
		resourceManager_.Shutdown();
		SDL_GL_DeleteContext(glContextPtr_);
		SDL_DestroyWindow(window_);
		SDL_Quit();
	}

	Engine& Engine::Get()
	{
		static Engine singleton;
		return singleton;
	}

	ResourceManager& Engine::GetResourceManager()
	{
		return resourceManager_;
	}

	Renderer& Engine::GetRenderer()
	{
		return renderer_;
	}

	void Engine::Run(I_Application* app)
	{
		using seconds = std::chrono::duration<float, std::ratio<1, 1>>;
		app_ = app;

		try
		{
			auto clock = std::chrono::system_clock::now();
			Init();
			while (true)
			{
				const auto start = std::chrono::system_clock::now();
				const auto dt = std::chrono::duration_cast<seconds>(start - clock);
				lastTimer_ = currentTimer_;
				currentTimer_ += dt.count();
				clock = start;
				{
					SDL_Event event;
					inputManager_.ClearQueues();
					while (SDL_PollEvent(&event))
					{
						if (event.type != SDL_QUIT)
						{
							inputManager_.StoreInput(event);
						}
						else
						{
							Shutdown();
							return;
						}
					}
				}
				app_->Update();
				renderer_.Update();
				SDL_GL_SwapWindow(window_);
			}
		}
		catch (const std::exception& e)
		{
			sge_ERROR(e.what());
		}
	}
	float Engine::GetCurrentTimer() const
	{
		return currentTimer_;
	}
	float Engine::GetLastTimer() const
	{
		return lastTimer_;
	}
	float Engine::GetDt() const
	{
		return currentTimer_ - lastTimer_;
	}
}//!sge