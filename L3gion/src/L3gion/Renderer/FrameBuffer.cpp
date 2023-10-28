#include "lgpch.h"
#include "L3gion/Renderer/FrameBuffer.h"
#include "L3gion/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace L3gion
{
	ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecs& specs)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
		{
			LG_CORE_ASSERT(false, "In RendererAPI create(): RendererAPI::None is not supported!");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: return createScope<OpenGLFrameBuffer>(specs);
		}

		LG_CORE_ASSERT(false, "In RendererAPI create(): Unknown RendererAPI!");
		return nullptr;
	}
}