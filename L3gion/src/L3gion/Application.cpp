#include "lgpch.h"
#include "Application.h"

#include "L3gion/Events/MouseEvent.h"

#include <GLFW/glfw3.h>

namespace L3gion
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::create());
		m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));
		m_Running = true;
	}

	Application::~Application()
	{

	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));

		LG_CORE_TRACE("{0}", e);
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

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}