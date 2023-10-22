#include "lgpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace L3gion
{
	ref<Shader> Shader::create(const std::string& filePath)
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:
			{
				LG_CORE_ASSERT(false, "In Shader::create():  RendererAPI::None is not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return std::make_shared<OpenGLShader>(filePath);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	ref<Shader> Shader::create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:
			{
				LG_CORE_ASSERT(false, "In Shader::create():  RendererAPI::None is not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return std::make_shared<OpenGLShader>(vertexSrc, fragmentSrc);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}