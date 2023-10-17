#pragma once

#include "L3gion/Renderer/Buffer.h"

namespace L3gion
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual const BufferLayout& getLayout() const override
		{
			return m_Layout;
		}
		virtual void setLayout(const BufferLayout& layout) override
		{
			m_Layout = layout;
		}

	private:
		uint32_t m_BufferID;
		BufferLayout m_Layout;
	};

//---------------------------INDEX_BUFFER-------------------------------------

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual uint32_t getCount() const override{ return m_Count; }

	private:
		uint32_t m_BufferID;
		uint32_t m_Count;
	};
}