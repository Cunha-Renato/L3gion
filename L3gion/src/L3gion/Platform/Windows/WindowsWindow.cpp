#include "lgpch.h"
#include "WindowsWindow.h"

namespace L3gion
{
	static bool s_GLFWInitialized = false;

	Window* Window::create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		init(props);
	}
	
	WindowsWindow::~WindowsWindow() { shutdown(); }

	void WindowsWindow::init(const WindowProps& props)
	{
		m_Data.title = props.title;
		m_Data.width = props.width;
		m_Data.height = props.height;

		if (!s_GLFWInitialized)
		{
			LG_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			LG_CORE_ASSERT(success, "Could not initialize GLFW!");

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.width, (int)props.height, m_Data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(true);
	}

	void WindowsWindow::shutdown() { glfwDestroyWindow(m_Window); }

	void WindowsWindow::onUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::setVSync(bool enable)
	{
		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enable;
	}

	bool WindowsWindow::isVSync() const { return m_Data.VSync; }
}
