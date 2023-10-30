#pragma once

#include "L3gion/Core/Core.h"
#include "L3gion/Core/KeyCodes.h"
#include <glm/glm.hpp>

namespace L3gion
{	
	class Input
	{
	public:
		// Keycodes will be send as LG_KEY
		static bool isKeyPressed(LgKeys keycode);

		static bool isMouseButtonPressed(int button);
		static glm::vec2 getMousePos();
	};
}