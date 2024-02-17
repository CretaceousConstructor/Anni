#include "MouseInputManager.h"

namespace Anni
{
	MouseInputManager::MouseInputManager(GraphicsComponent& gfx_) :
		gfx(gfx_),
		gui_io{ ImGui::GetIO() }
	{
	}

	float MouseInputManager::GetPitchDiff() const
	{
		return gui_io.MouseDelta.y;
	}

	float MouseInputManager::GetYawDiff() const
	{
		return gui_io.MouseDelta.x;
	}

	float MouseInputManager::GetMouseScroll() const
	{
		return 0.1f * gui_io.MouseWheel;
	}

}        // namespace Anni
