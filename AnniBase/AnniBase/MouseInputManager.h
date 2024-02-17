#pragma once
#include "GraphicsComponent.h"
#include "VkImgui.h"
namespace Anni
{

	class MouseInputManager
	{
	public:
		MouseInputManager(GraphicsComponent& gfx_);
		[[nodiscard]] float GetPitchDiff() const;
		[[nodiscard]] float GetYawDiff() const;
		[[nodiscard]] float GetMouseScroll() const;

	private:
		GraphicsComponent& gfx;

	private:
		ImGuiIO& gui_io;
	};

}
