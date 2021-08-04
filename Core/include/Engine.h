#pragma once

#include "Renderer.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "macros.h"

struct SDL_Window;

namespace sge
{
	class I_Application
	{
	public:
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
	};

	class Engine
	{
		using SDL_GLContext = void*;

		SDL_Window* window_ = nullptr;
		SDL_GLContext glContextPtr_ = nullptr;
		float lastTimer_ = 0.0f;
		float currentTimer_ = 0.0f;
		Renderer renderer_ = {};
		InputManager inputManager_ = {};
		ResourceManager resourceManager_ = {};
		I_Application* app_ = nullptr;

	private:
		void Init();
		void Shutdown();
		sge_ALLOW_CONSTRUCTION(Engine);
	public:
		sge_DISALLOW_COPY(Engine);
		static Engine& Get();
		ResourceManager& GetResourceManager();
		void Run(I_Application* app);
		float GetCurrentTimer() const;
		float GetLastTimer() const;
		float GetDt() const;
	};
} //!sge