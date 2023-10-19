#pragma once

#include "L3gion/Renderer/VertexArray.h"

namespace L3gion
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		virtual ~OpenGLVertexArray() {}

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

	};
}

