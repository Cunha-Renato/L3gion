#pragma once

#include "L3gion/Core.h"

// TODO: Change mouse position to (probably) glm::vec2
namespace L3gion
{	
	class L3GION_API Input
	{
	public:
		// Keycodes will be send as LG_KEY
		inline static bool isKeyPressed(int keycode) 
		{ 
			return s_Instance->isKeyPressedImpl(keycode); 
		}

		inline static bool isMouseButtonPressed(int button)
		{
			return s_Instance->isMouseButtonPressedImpl(button);
		}
		inline static std::pair<float, float> getMousePos()
		{
			return s_Instance->getMousePosImpl();
		}
	
	protected:
		virtual bool isKeyPressedImpl(int keycode) = 0;

		virtual bool isMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> getMousePosImpl() = 0;
	
	private:
		static Input* s_Instance;
	};
}