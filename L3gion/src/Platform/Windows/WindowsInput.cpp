#include "lgpch.h"

#include "L3gion/Core/Input.h"

#include "L3gion/Core/Application.h"
#include <GLFW/glfw3.h>

namespace L3gion
{
	bool Input::isKeyPressed(LgKey keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		auto state = glfwGetKey(window, LG_KEY_TO_GLFW(keycode));

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		
		auto state = glfwGetMouseButton(window, button);
		
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::getMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return glm::vec2(xPos, yPos);
	}
}