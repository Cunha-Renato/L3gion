#include "lgpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace L3gion
{
	ref<VertexArray> VertexArray::create()
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:
			{
				LG_CORE_ASSERT(false, "In VertexBuffer::create() RendererAPI::None is not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return std::make_shared<OpenGLVertexArray>();
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}