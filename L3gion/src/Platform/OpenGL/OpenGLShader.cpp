#include "lgpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace L3gion
{
	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) : m_ShaderID(0)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			LG_CORE_ERROR("{0}", infoLog.data());
			LG_CORE_ASSERT(false, "Vertex shader compilation failed!");

			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			LG_CORE_ERROR("{0}", infoLog.data());
			LG_CORE_ASSERT(false, "Fragment shader compilation failed!");

			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_ShaderID = glCreateProgram();
		GLuint program = m_ShaderID;

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

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
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			LG_CORE_ERROR("{0}", infoLog.data());
			LG_CORE_ASSERT(false, "Shader linking failed!");

			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	}
	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_ShaderID);
	}

	void OpenGLShader::bind() const
	{
		glUseProgram(m_ShaderID);
	}
	void OpenGLShader::unbind() const
	{
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
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In setFloat: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform1i(location, value);
	}
	void OpenGLShader::setFloat(const std::string& name, float value)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In setFloat: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform1f(location, value);
	}
	void OpenGLShader::setFloat2(const std::string& name, glm::vec2 values)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In setFloat2: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform2f(location, values.x, values.y);
	}
	void OpenGLShader::setFloat3(const std::string& name, glm::vec3 values)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In setFloat3: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform3f(location, values.x, values.y, values.z);
	}
	void OpenGLShader::setFloat4(const std::string& name, glm::vec4 values)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In setFloat4: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::setMat3(const std::string& name, glm::mat3 matrix)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In setMat3: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::setMat4(const std::string& name, glm::mat4 matrix)
	{
		int location = getUniformLocation(name);

		if (location == -1)
		{
			LG_CORE_ERROR("In setMat4: Uniform {0} doesn't exist!", name.c_str());
			LG_CORE_ASSERT(false, "Inexistant Uniform!");
		}

		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
