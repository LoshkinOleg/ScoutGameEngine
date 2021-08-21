#pragma once

#include "ResourceManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "PhysicsEngine.h"

struct SDL_Window;

namespace sge
{
	class Engine;

	class I_Application
	{
	public:
		virtual void Init(Engine& engine) = 0;
		virtual void Update(Engine& engine) = 0;
		virtual void Shutdown() = 0;
	};

	class Engine
	{
	public:
		Engine() = delete;
		Engine(I_Application& app): app_(app){};

		static Engine& Get();
		ResourceManager& GetResourceManager();
		Renderer& GetRenderer();
		InputManager& GetInputManager();
		PhysicsEngine& GetPhysicsEngine();
		
		float GetCurrentTimer() const;
		float GetLastTimer() const;
		float GetDt() const;

		void Run();

	private:
		SDL_Window* window_ = nullptr;
		void* glContext_ = nullptr;
		float lastTimer_ = 0.0f;
		float currentTimer_ = 0.0f;
		I_Application& app_;
		ResourceManager rm_;
		Renderer renderer_;
		InputManager im_;
		PhysicsEngine phy_;

		void Init_();
		void Shutdown_();
	};
} //!sge