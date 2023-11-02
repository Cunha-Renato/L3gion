#include "lgpch.h"
#include "L3gion/Renderer/FrameBuffer.h"
#include "L3gion/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace L3gion
{
	ref<Framebuffer> Framebuffer::create(const FramebufferSpecs& specs)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
		{
			LG_CORE_ASSERT(false, "In RendererAPI create(): RendererAPI::None is not supported!");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: return createScope<OpenGLFramebuffer>(specs);
		}

		LG_CORE_ASSERT(false, "In RendererAPI create(): Unknown RendererAPI!");
		return nullptr;
	}
}