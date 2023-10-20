#include "lgpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace L3gion
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case L3gion::ShaderDataType::Float:		return GL_FLOAT;
			case L3gion::ShaderDataType::Float2:	return GL_FLOAT;
			case L3gion::ShaderDataType::Float3:	return GL_FLOAT;
			case L3gion::ShaderDataType::Float4:	return GL_FLOAT;
			case L3gion::ShaderDataType::Mat3:		return GL_FLOAT;
			case L3gion::ShaderDataType::Mat4:		return GL_FLOAT;
			case L3gion::ShaderDataType::Int:		return GL_INT;
			case L3gion::ShaderDataType::Int2:		return GL_INT;
			case L3gion::ShaderDataType::Int3:		return GL_INT;
			case L3gion::ShaderDataType::Int4:		return GL_INT;
			case L3gion::ShaderDataType::Bool:		return GL_BOOL;
		}

		LG_CORE_ASSERT(false, "In ShaderDataTypeToOpenGLBaseType(): Unknown ShaderDataType!");

		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() 
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::bind() const
	{
		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::unbind() const
	{
		glBindVertexArray(0);

	}

	void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		LG_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "In addVertexBuffer: VertexBuffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->getLayout();
		for (const auto& element : layout)
		{
			// Structure of the data
			glEnableVertexAttribArray(index);

			glVertexAttribPointer(index,
				element.getComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*)element.offset
			);

			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}
	void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->bind();

		m_IndexBuffer = indexBuffer;
	}
}