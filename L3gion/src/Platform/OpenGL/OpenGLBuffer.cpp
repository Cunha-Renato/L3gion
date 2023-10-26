#include "lgpch.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

#include <glad/glad.h>

namespace L3gion
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		LG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		LG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		LG_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLVertexBuffer::bind() const
	{
		LG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}
	void OpenGLVertexBuffer::unbind() const
	{
		LG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::setData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

//--------------------------- INDEX_BUFFER-------------------------------------

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		LG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		LG_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLIndexBuffer::bind() const
	{
		LG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}
	void OpenGLIndexBuffer::unbind() const
	{
		LG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}