#include "lgpch.h"

#include "L3gion/Renderer/Shader.h"
#include "L3gion/Renderer/Renderer.h"

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
				return createRef<OpenGLShader>(filePath);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	ref<Shader> Shader::create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
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
				return createRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
			}
		}

		LG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

//------------------------- SHADER_LIB ---------------------------

	void ShaderLib::add(const ref<Shader>& shader)
	{
		auto& name = shader->getName();

		add(name, shader);
	}
	void ShaderLib::add(const std::string& name, const ref<Shader>& shader)
	{
		LG_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "In Shader add(): Shader already exists!");

		m_Shaders[name] = shader;
	}

	ref<Shader> ShaderLib::load(const std::string& filePath)
	{
		auto shader = Shader::create(filePath);
		add(shader);
		return shader;
	}
	ref<Shader> ShaderLib::load(const std::string& name, const std::string& filePath)
	{
		auto shader = Shader::create(filePath);
		add(name, shader);
		return shader;
	}

	ref<Shader> ShaderLib::get(const std::string& name)
	{
		LG_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "In Shader get(): Shader not found!");

		return m_Shaders[name];
	}
}