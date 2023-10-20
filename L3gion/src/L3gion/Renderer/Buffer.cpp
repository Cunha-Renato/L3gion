#include "lgpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace L3gion
{
	VertexBuffer* VertexBuffer::create(float* vertices, uint32_t size)
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
				return new OpenGLVertexBuffer(vertices, size);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	
	IndexBuffer* IndexBuffer::create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:
			{
				LG_CORE_ASSERT(false, "In IndexBuffer::create() RendererAPI::None is not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return new OpenGLIndexBuffer(indices, count);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}