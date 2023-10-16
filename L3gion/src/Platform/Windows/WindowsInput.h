#pragma once

#include "L3gion/Input.h"
#include "L3gion/KeyCodes.h"

namespace L3gion
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool isKeyPressedImpl(int keycode) override;

		virtual bool isMouseButtonPressedImpl(int button) override;
		virtual glm::vec2 getMousePosImpl() override;
	};
}