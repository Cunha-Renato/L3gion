#include "lgpch.h"
#include "WindowsInput.h"

#include "L3gion/Core/Application.h"
#include <GLFW/glfw3.h>

namespace L3gion
{
	scope<Input> Input::s_Instance = createScope<WindowsInput>();

	bool WindowsInput::isKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		auto state = glfwGetKey(window, LgKeyToGlfw(keycode));

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::isMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		
		auto state = glfwGetMouseButton(window, button);
		
		return state == GLFW_PRESS;
	}

	glm::vec2 WindowsInput::getMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return glm::vec2(xPos, yPos);
	}
}