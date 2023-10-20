#include "lgpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace L3gion
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}