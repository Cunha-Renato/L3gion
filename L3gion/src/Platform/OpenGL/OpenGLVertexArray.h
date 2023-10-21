#pragma once

#include "L3gion/Renderer/VertexArray.h"

namespace L3gion
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray::OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers() const override
		{
			return m_VertexBuffers;
		}
		virtual const ref<IndexBuffer>& getIndexBuffer() const override
		{
			return m_IndexBuffer;
		}

	private:
		uint32_t m_RendererID;
		std::vector<ref<VertexBuffer>> m_VertexBuffers;
		ref<IndexBuffer> m_IndexBuffer;
	};
}

