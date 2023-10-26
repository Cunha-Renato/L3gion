#include "lgpch.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace L3gion
{
	static GLenum shaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		LG_CORE_ASSERT(false, "In ShaderTypeFromString(): Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		LG_PROFILE_FUNCTION();
		
		std::string source = readFile(filePath);
		auto shaderSrcs = preProcess(source);
		compile(shaderSrcs);
	
		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		// Extract name from filePath
		auto lastDot = filePath.rfind(".");
		auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filePath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : m_ShaderID(0), m_Name(name)
	{
		LG_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		compile(sources);
	}
	OpenGLShader::~OpenGLShader()
	{
		LG_PROFILE_FUNCTION();

		glDeleteProgram(m_ShaderID);
	}

	std::string OpenGLShader::readFile(const std::string& filePath)
	{
		LG_PROFILE_FUNCTION();
		
		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		
		if (in)
		{
			size_t size = in.tellg();
			if (size != -1)
			{
				in.seekg(0, std::ios::end);
				result.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&result[0], result.size());
				in.close();
			}
			else
				LG_CORE_ASSERT(false, "In OpenGLShader readFile(): Could not read from File!");
			
		}
		else
			LG_CORE_WARN("In readFile(): Could not open shader file {0}", filePath.c_str());

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string& source)
	{
		LG_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSrcs;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			LG_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			LG_CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSrcs[shaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSrcs;
	}

	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSrcs)
	{
		LG_PROFILE_FUNCTION();

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		GLuint program = glCreateProgram();
		LG_CORE_ASSERT(shaderSrcs.size() <= 2, "In OpenGLShader compile(), only supports 2 shaders!");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSrcs)
		{
			GLenum shaderType = kv.first;
			const std::string& source = kv.second;

			// Create an empty vertex shader handle
			GLuint shader = glCreateShader(shaderType);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				LG_CORE_ERROR("{0}", infoLog.data());
				LG_CORE_ASSERT(false, "In: OpenGLShader compile(): Shader compilation failed!");

				return;
			}

			// Attach our shaders to our program
			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : glShaderIDs)
				glDeleteShader(id);

			LG_CORE_ERROR("{0}", infoLog.data());
			LG_CORE_ASSERT(false, "Shader linking failed!");

			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_ShaderID = program;
	}

	void OpenGLShader::bind() const
	{
		LG_PROFILE_FUNCTION();

		glUseProgram(m_ShaderID);
	}
	void OpenGLShader::unbind() const
	{
		LG_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	int OpenGLShader::getUniformLocation(const std::string& name) const
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		m_UniformLocationCache[name] = location;

		return location;
	}

	void OpenGLShader::setInt(const std::string& name, int value)
	{
		uploadUniformInt(name, value);
	}
	void OpenGLShader::setIntArray(const std::string& name, int* values, uint32_t count)
	{
		uploadUniformIntArray(name, values, count);
	}
	void OpenGLShader::setFloat(const std::string& name, float value)
	{
		uploadUniformFloat(name, value);
	}
	void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& value)
	{
		uploadUniformFloat3(name, value);
	}
	void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value)
	{
		uploadUniformFloat4(name, value);
	}
	void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value)
	{
		uploadUniformMat4(name, value);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, int value)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In uploadUniformFloat: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform1i(location, value);
	}
	void OpenGLShader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		int location = getUniformLocation(name);
		glUniform1iv(location, count, values);
	}
	void OpenGLShader::uploadUniformFloat(const std::string& name, float value)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In uploadUniformFloat: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform1f(location, value);
	}
	void OpenGLShader::uploadUniformFloat2(const std::string& name, glm::vec2 values)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In uploadUniformFloat2: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform2f(location, values.x, values.y);
	}
	void OpenGLShader::uploadUniformFloat3(const std::string& name, glm::vec3 values)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In uploadUniformFloat3: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform3f(location, values.x, values.y, values.z);
	}
	void OpenGLShader::uploadUniformFloat4(const std::string& name, glm::vec4 values)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In uploadUniformFloat4: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::uploadUniformMat3(const std::string& name, glm::mat3 matrix)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In uploadUniformMat3: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::uploadUniformMat4(const std::string& name, glm::mat4 matrix)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In uploadUniformMat4: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
