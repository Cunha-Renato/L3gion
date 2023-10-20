#pragma once

#include <string>

#include <glm/glm.hpp>

namespace L3gion
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void bind() const;
		void unbind() const;

		void setMat4(const std::string& name, glm::mat4 matrix);
	public:
		uint32_t m_ShaderID;
	};
}
