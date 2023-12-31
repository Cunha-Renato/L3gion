#include "lgpch.h"

#include "Platform/Windows/WindowsWindow.h"

#include <glad/glad.h>

#include "L3gion/Events/ApplicationEvent.h"
#include "L3gion/Events/KeyEvent.h"
#include "L3gion/Events/MouseEvent.h"
#include "L3gion/Core/KeyCodes.h"
#include "L3gion/Renderer/Renderer.h"
#include <GLFW/glfw3native.h>

namespace L3gion
{
	static uint8_t s_GLFWwindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	scope<Window> Window::create(const WindowProps& props)
	{
		return createScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		init(props);
	}
	
	WindowsWindow::~WindowsWindow() 
	{ 
		LG_PROFILE_FUNCTION();

		shutdown(); 
	}

	void WindowsWindow::init(const WindowProps& props)
	{
		LG_PROFILE_FUNCTION();
		m_Data.title = props.title;
		m_Data.width = props.width;
		m_Data.height = props.height;

		LG_CORE_INFO("Initializing WindowsWindow!");

		// Initializing GLFW
		if (s_GLFWwindowCount == 0)
		{
			LG_PROFILE_SCOPE("GLFW_Init");
			int success = glfwInit();
			LG_CORE_ASSERT(success, "Could not initialize GLFW!");

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			LG_PROFILE_SCOPE("GLFW_CreateWindow");

#if defined(LG_DEBUG)
			if (Renderer::getAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
			m_Window = glfwCreateWindow((int)props.width, (int)props.height, m_Data.title.c_str(), NULL, NULL);
			LG_CORE_ASSERT(m_Window, "In WindowsWindow Init(): Failed to create a window!");

			++s_GLFWwindowCount;
		}

		m_Context = GraphicsContext::create(m_Window);
		m_Context->init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(true);

		// Set GLFW callbacks
		// RESIZE
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		// CLOSE
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		// KEY
		// Keys need to be converted from GLFW_KEY to LG_KEY
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(LG_GLFW_KEY_TO_LG(key), 0);
					data.EventCallback(event);

					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(LG_GLFW_KEY_TO_LG(key), 1);
					data.EventCallback(event);

					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(LG_GLFW_KEY_TO_LG(key));
					data.EventCallback(event);

					break;
				}
				default:
					break;
			}
		});
	
		// KEY_TYPED
		// Keys need to be converted from GLFW_KEY to LG_KEY
		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keyCode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keyCode);
			data.EventCallback(event);
		});

		// MOUSE_BTN
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);

					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);

					break;
				}
				default:
					break;
			}
		});
	
		// SCROLL
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrollEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		// MOUSE_MOVE
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::shutdown() 
	{ 
		LG_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window); 
		--s_GLFWwindowCount;

		if(s_GLFWwindowCount == 0)
			glfwTerminate();
	}

	void WindowsWindow::onUpdate()
	{
		LG_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->swapBuffers();
	}

	void WindowsWindow::setVSync(bool enable = true)
	{
		LG_PROFILE_FUNCTION();

		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enable;
	}

	bool WindowsWindow::isVSync() const { return m_Data.VSync; }
}
