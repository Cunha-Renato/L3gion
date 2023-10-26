#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

namespace L3gion
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setInt(const std::string& name, int value) = 0;
		virtual void setIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void setFloat(const std::string& name, float value) = 0;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual const std::string& getName() const = 0;

		static ref<Shader> create(const std::string& filePath);
		static ref<Shader> create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

//------------------------- SHADER_LIB ---------------------------

	class ShaderLib
	{
	public:
		void add(const ref<Shader>& shader);
		void add(const std::string& name, const ref<Shader>& shader);
		ref<Shader> load(const std::string& filePath);
		ref<Shader> load(const std::string& name, const std::string& filePath);

		ref<Shader> get(const std::string& name);

	private:
		std::unordered_map<std::string, ref<Shader>> m_Shaders;
	};
}
