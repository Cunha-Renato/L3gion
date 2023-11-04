#include "lgpch.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "L3gion/Core/Timer.h"

#include <fstream>
#include <filesystem>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>


namespace L3gion
{
	namespace Utils
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

		static shaderc_shader_kind glShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER: return shaderc_glsl_vertex_shader;
				case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			LG_CORE_ASSERT(false, "Unknown Shader stage!");
			return (shaderc_shader_kind)0;
		}

		static const char* glShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			LG_CORE_ASSERT(false, "Unknown Shader stage!");
			return nullptr;
		}

		static const char* getCacheDirectory()
		{
			return "resources/cache/shader/opengl";
		}

		static void createCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = getCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* glShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER: return ".cached_opengl.vert";
				case GL_FRAGMENT_SHADER: return ".cached_opengl.frag";
			}
			LG_CORE_ASSERT(false, "Unknown Shader stage!");
			return "";
		}

		static const char* glShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER: return ".cached_vulkan.vert";
				case GL_FRAGMENT_SHADER: return ".cached_vulkan.frag";
			}
			LG_CORE_ASSERT(false, "Unknown Shader stage!");
			return "";
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		LG_PROFILE_FUNCTION();
		
		Utils::createCacheDirectoryIfNeeded();

		std::string source = readFile(filePath);
		auto shaderSrcs = preProcess(source);
		
		{
			Timer timer;
			compileOrGetVulkanBinaries(shaderSrcs);
			compileOrGetOpenGLBinaries();
			createProgram();
			LG_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}
	
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

		compileOrGetVulkanBinaries(sources);
		compileOrGetOpenGLBinaries();
		createProgram();
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
			LG_CORE_ASSERT(Utils::shaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSrcs[Utils::shaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSrcs;
	}

	void OpenGLShader::compileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSrcs)
	{
		LG_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::getCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSrcs)
		{
			std::filesystem::path shaderFilePath = m_Filepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::glShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::glShaderStageToShaderC(stage), m_Filepath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					LG_CORE_ERROR(module.GetErrorMessage());
					LG_CORE_ASSERT(false, " ");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			reflect(stage, data);
	}

	void OpenGLShader::compileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::getCacheDirectory();
		shaderData.clear();
		m_OpenGLSourceCode.clear();

		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilepath = m_Filepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilepath.filename().string() + Utils::glShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::glShaderStageToShaderC(stage), m_Filepath.c_str());

				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					LG_CORE_ERROR(module.GetErrorMessage().c_str());
					LG_CORE_ASSERT(false, " ");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);

				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::createProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			LG_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_Filepath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_ShaderID = program;
	}

	void OpenGLShader::reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		LG_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::glShaderStageToString(stage), m_Filepath);
		LG_CORE_TRACE("{0} uniform buffers", resources.uniform_buffers.size());
		LG_CORE_TRACE("{0} resources", resources.sampled_images.size());

		LG_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			LG_CORE_TRACE("{0}", resource.name);
			LG_CORE_TRACE("size = {0}", bufferSize);
			LG_CORE_TRACE("Binding = {0}", binding);
			LG_CORE_TRACE("Members = {0}", memberCount);
		}
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
