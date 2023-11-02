#pragma once

#include "L3gion/Renderer/Framebuffer.h"

namespace L3gion
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecs& specs);
		virtual ~OpenGLFramebuffer();
	
		void invalidate();

		virtual void bind() override;
		virtual void unbind() override;

		virtual void resize(uint32_t width, uint32_t height) override;
		virtual int readPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual const FramebufferSpecs getSpecification() const override { return m_Specification; }

		virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const override 
		{	
			LG_CORE_ASSERT(index < m_ColorAttachments.size(), " ");
			return m_ColorAttachments[index]; 
		}

	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecs m_Specification;

		std::vector<FramebufferTextureSpecs> m_ColorAttachmentSpecs;
		FramebufferTextureSpecs m_DepthAttachmentSpec = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}
