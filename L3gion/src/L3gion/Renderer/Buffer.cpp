#include "lgpch.h"

#include "L3gion/Renderer/Buffer.h"
#include "L3gion/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace L3gion
{
	ref<VertexBuffer> VertexBuffer::create(uint32_t size)
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
			return createRef<OpenGLVertexBuffer>(size);
		}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size)
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
				return createRef<OpenGLVertexBuffer>(vertices, size);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count)
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
				return createRef<OpenGLIndexBuffer>(indices, count);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}