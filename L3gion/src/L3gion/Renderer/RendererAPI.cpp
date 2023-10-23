#include "lgpch.h"

#include "L3gion/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace L3gion
{
	RendererAPI::API RendererAPI::s_API= RendererAPI::API::OpenGL;

	scope<RendererAPI> RendererAPI::create()
	{
		switch (s_API)
		{
			case RendererAPI::API::None:
			{
				LG_CORE_ASSERT(false, "In RendererAPI create(): RendererAPI::None is not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL: return createScope<OpenGLRendererAPI>();
		}

		LG_CORE_ASSERT(false, "In RendererAPI create(): Unknown RendererAPI!");
		return nullptr;
	}
}