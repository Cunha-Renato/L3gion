#include "lgpch.h"
#include "L3gion/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace L3gion
{
	scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::create();
}