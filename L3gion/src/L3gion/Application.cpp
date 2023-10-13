#include "lgpch.h"
#include "Application.h"

#include "L3gion/Events/MouseEvent.h"

#include <GLFW/glfw3.h>

namespace L3gion
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::create());
		m_Running = true;
	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		MouseButtonPressedEvent e(15);
		("alow");


		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->onUpdate();
		}
	}
}