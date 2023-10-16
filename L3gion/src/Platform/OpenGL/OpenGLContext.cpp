#include "lgpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace L3gion
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		LG_CORE_ASSERT(m_WindowHandle, "OpenGL Handle is null!");
	}
	OpenGLContext::~OpenGLContext()
	{

	}

	void OpenGLContext::init()
	{
		// Initializing GLAD
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LG_CORE_ASSERT(status, "Failed to initialize GLAD!")

		LG_CORE_INFO("OpenGL INFO:");
		LG_CORE_INFO("Vendor:   {0}", (const char*)glGetString(GL_VENDOR));
		LG_CORE_INFO("Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		LG_CORE_INFO("Version:  {0}", (const char*)glGetString(GL_VERSION));
	}
	void OpenGLContext::swapBuffers()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_WindowHandle);
	}
}