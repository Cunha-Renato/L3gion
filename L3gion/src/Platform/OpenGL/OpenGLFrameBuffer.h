#pragma once

#include "L3gion/Renderer/FrameBuffer.h"

namespace L3gion
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecs& specs);
		virtual ~OpenGLFrameBuffer();
	
		void invalidate();

		virtual void bind() override;
		virtual void unbind() override;

		virtual void resize(uint32_t width, uint32_t height) override;

		virtual const FrameBufferSpecs getSpecification() const override { return m_Specification; }

		virtual uint32_t getColorAttachmentRendererID() const override { return m_ColorAttachment; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FrameBufferSpecs m_Specification;
	};

}
