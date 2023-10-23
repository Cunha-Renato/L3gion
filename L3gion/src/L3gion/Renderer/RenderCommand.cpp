#include "lgpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace L3gion
{
	scope<RendererAPI> RenderCommand::s_RendererAPI = createScope<OpenGLRendererAPI>();
}