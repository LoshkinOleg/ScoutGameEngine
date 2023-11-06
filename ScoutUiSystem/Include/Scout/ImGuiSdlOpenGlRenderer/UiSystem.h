#pragma once

#include <imgui.h>

#include <Scout/IUiSystem.h>
#include <Scout/IInputSystem.h>
#include <Scout/IGraphicEngine.h>

namespace Scout
{
	class UiSystem_ImGui final : public IImmediateModeUiSystem
	{
	public:
		void Render() override;
		bool ProcessEvent(const void* event) override;
		void RegisterDrawingCallback(ImmediateModeUiDrawingCallback callback) override;

		UiSystem_ImGui(IWindow* window, IGraphicsEngine* renderer);
		~UiSystem_ImGui();

	private:
		std::vector<ImmediateModeUiDrawingCallback> callbacks_{};
		ImGuiContext* pContext_ = nullptr;
	};
}