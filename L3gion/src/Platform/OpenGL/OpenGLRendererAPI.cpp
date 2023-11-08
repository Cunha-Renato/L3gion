#include "lgpch.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace L3gion
{
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         LG_CORE_CRITICAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       LG_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW:          LG_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: LG_CORE_TRACE(message); return;
		}

		LG_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::init()
	{
		LG_PROFILE_FUNCTION();

#ifdef LG_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}
	void OpenGLRendererAPI::setViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndexed(const ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = (indexCount == 0) ? vertexArray->getIndexBuffer()->getCount() : indexCount;
		vertexArray->bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::drawLines(const ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::setLineThickness(float thickness)
	{
		glLineWidth(thickness);
	}
}