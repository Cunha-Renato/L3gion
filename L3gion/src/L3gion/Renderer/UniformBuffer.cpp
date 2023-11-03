#include "lgpch.h"
#include "L3gion/Renderer/UniformBuffer.h"

#include "L3gion/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace L3gion
{
	ref<UniformBuffer> UniformBuffer::create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None: 
			{
				LG_CORE_ASSERT(false, "RendererAPI::None is not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return createRef<OpenGLUniformBuffer>(size, binding);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}