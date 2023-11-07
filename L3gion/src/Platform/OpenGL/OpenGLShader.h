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
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setInt(const std::string& name, int value) override;
		virtual void setIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void setFloat(const std::string& name, float value) override;
		virtual void setFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void setMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& getName() const override { return m_Name; }

		void uploadUniformInt(const std::string& name, int value);
		void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void uploadUniformFloat(const std::string& name, float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& value);
		
		void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	
	private:
		std::string readFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
		void compileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSrcs);
		void compileOrGetOpenGLBinaries();
		void createProgram();
		void reflect(GLenum stage, const std::vector<uint32_t>& shaderData);
	
	private:
		uint32_t m_ShaderID;
		std::string m_Name;
		std::string m_Filepath;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;
		mutable std::unordered_map<std::string, GLenum> m_UniformLocationCache;

		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
	};
}
