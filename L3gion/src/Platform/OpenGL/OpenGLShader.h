#pragma once

#include "L3gion/Renderer/Shader.h"
#include <glm/glm.hpp>

#include <unordered_map>

//TODO: REMOVE!
typedef unsigned int GLenum;

namespace L3gion
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		void setInt(const std::string& name, int value);
		void setFloat(const std::string& name, float value);
		void setFloat2(const std::string& name, glm::vec2 values);
		void setFloat3(const std::string& name, glm::vec3 values);
		void setFloat4(const std::string& name, glm::vec4 values);
		
		void setMat3(const std::string& name, glm::mat3 matrix);
		void setMat4(const std::string& name, glm::mat4 matrix);
	public:
		uint32_t m_ShaderID;

	private:
		std::string readFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
		void compile(const std::unordered_map<GLenum, std::string>& shaderSrcs);
		int getUniformLocation(const std::string& name) const;
	
	private:
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;
	};
}
