#pragma once

#include "L3gion/Core/Core.h"
#include "glm/glm.hpp"

namespace L3gion
{	
	class Input
	{
	protected:
		Input() = default;

	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		// Keycodes will be send as LG_KEY
		inline static bool isKeyPressed(int keycode) 
		{ 
			return s_Instance->isKeyPressedImpl(keycode); 
		}

		inline static bool isMouseButtonPressed(int button)
		{
			return s_Instance->isMouseButtonPressedImpl(button);
		}
		inline static glm::vec2 getMousePos()
		{
			return s_Instance->getMousePosImpl();
		}
	
	protected:
		virtual bool isKeyPressedImpl(int keycode) = 0;

		virtual bool isMouseButtonPressedImpl(int button) = 0;
		virtual glm::vec2 getMousePosImpl() = 0;
	
	private:
		static scope<Input> s_Instance;
	};
}