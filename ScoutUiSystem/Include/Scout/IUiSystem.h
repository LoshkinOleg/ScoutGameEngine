#pragma once

#include <Scout/TypedefAndEnum.h>
#include <Scout/IInputSystem.h>
#include <Scout/IGraphicEngine.h>

namespace Scout
{
	class IImmediateModeUiSystem
	{
	public:
		virtual void Render() = 0;
		virtual bool ProcessEvent(const void* event) = 0;
		/*
		Use this method to define your own UI elements to draw.
		*/
		virtual void RegisterDrawingCallback(ImmediateModeUiDrawingCallback callback) = 0;
	};

	struct UiSystemDef
	{
		IWindow* pInputsProvider = nullptr;
		IGraphicsEngine* pRenderer = nullptr;
		UiApi implementation = UiApi::NONE;
	};

	/*
		Factory function for creating immediate mode UI system implementations.
	*/
	std::unique_ptr<IImmediateModeUiSystem> MakeImmediateModeUiSystem(const UiSystemDef def);
}