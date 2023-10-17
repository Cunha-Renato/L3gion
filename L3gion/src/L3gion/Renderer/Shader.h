#pragma once

#include <string>

namespace L3gion
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void bind() const;
		void unbind() const;
	public:
		uint32_t m_ShaderID;
	};
}
