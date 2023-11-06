#include <Scout/IUiSystem.h>

#include <stdexcept>

#include <Scout/ImGuiSdlOpenGlRenderer/UiSystem.h>

namespace Scout
{
	std::unique_ptr<IImmediateModeUiSystem> MakeImmediateModeUiSystem(const UiSystemDef def)
	{
		return std::make_unique<UiSystem_ImGui>(def.pInputsProvider, def.pRenderer);
		throw std::runtime_error("MakeImmediateModeUiSystem: Implement this");
	}
}