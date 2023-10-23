#include "lgpch.h"

#include "L3gion/Renderer/GraphicsContext.h"
#include "L3gion/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace L3gion
{
	scope<GraphicsContext> GraphicsContext::create(void* window)
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:
			{
				LG_CORE_ASSERT(false, "In GraphicsContext create(): RendererAPI::None is not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL: return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		LG_CORE_ASSERT(false, "In GraphicsContext create(): Unknown RendererAPI");
		return nullptr;
	}
}