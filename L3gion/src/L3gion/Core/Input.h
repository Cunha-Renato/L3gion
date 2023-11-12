#pragma once

#include "L3gion/Core/Core.h"
#include "L3gion/Core/KeyCodes.h"
#include "L3gion/Core/MouseButtonCodes.h"
#include <glm/glm.hpp>

namespace L3gion
{	
	class Input
	{
	public:
		// Keycodes will be send as LG_KEY
		static bool isKeyPressed(LgKey keycode);

		static bool isMouseButtonPressed(int button);
		static glm::vec2 getMousePos();
	};
}