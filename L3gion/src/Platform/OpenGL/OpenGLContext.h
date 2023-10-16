#pragma once

#include "L3gion/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace L3gion
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		~OpenGLContext();

		virtual void init() override;
		virtual void swapBuffers() override;
	
	private:
		GLFWwindow* m_WindowHandle;
	};
}