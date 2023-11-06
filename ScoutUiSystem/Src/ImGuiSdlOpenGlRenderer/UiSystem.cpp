#include <Scout/ImGuiSdlOpenGlRenderer/UiSystem.h>

#include <stdexcept>

#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

namespace Scout
{
	void UiSystem_ImGui::Render()
	{
		for (auto& callback : callbacks_)
		{
			ImGui_ImplSDLRenderer_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			callback();
		}

		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	}

	void UiSystem_ImGui::RegisterDrawingCallback(ImmediateModeUiDrawingCallback callback)
	{
		callbacks_.push_back(callback);
	}

	bool UiSystem_ImGui::ProcessEvent(const void* event)
	{
		assert(pContext_);
		if (event == nullptr) throw std::runtime_error("UiSystem_ImGui::ImGuiProcessSdlEvent: event passed is a nullptr.");

		if (!ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard) return false;
		return ImGui_ImplSDL2_ProcessEvent(static_cast<const SDL_Event*>(event));
	}

	UiSystem_ImGui::UiSystem_ImGui(IWindow* window, IGraphicsEngine* renderer)
	{
		if (!window || !renderer)
		{
			throw std::runtime_error("UiSystem_ImGui::UiSystem_ImGui: window and/or renderer parameters are nullptrs");
		}

		IMGUI_CHECKVERSION();
		pContext_ = ImGui::CreateContext();
		if (!pContext_)
		{
			throw std::runtime_error("UiSystem_ImGui::UiSystem_ImGui: ImGui failed to create a context.");
		}
		auto success = ImGui_ImplSDL2_InitForSDLRenderer(static_cast<SDL_Window*>(window->GetImplementationApi()), static_cast<SDL_Renderer*>(renderer->GetImplementationApi()));
		if (!success)
		{
			throw std::runtime_error("UiSystem_ImGui::UiSystem_ImGui: ImGui_ImplSDL2_InitForSDLRenderer call failed, did you pass Window_SDL_OpenGL33* as window and GraphicsEngine_SDL* as renderer, as the implementation expects?");
		}
		success = ImGui_ImplSDLRenderer_Init(static_cast<SDL_Renderer*>(renderer->GetImplementationApi()));
		if (!success)
		{
			throw std::runtime_error("UiSystem_ImGui::UiSystem_ImGui: ImGui_ImplSDLRenderer_Init call failed, did you pass GraphicsEngine_SDL* as renderer, as the implementation expects?");
		}

		window->RegisterUiProcessEventCallback([&](const void* event)->bool { return ProcessEvent(event); });
		renderer->RegisterUiDrawingCallback([&]() { Render(); });
	}

	UiSystem_ImGui::~UiSystem_ImGui()
	{
		if (pContext_)
		{
			ImGui_ImplSDLRenderer_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext(pContext_);
		}
	}
}