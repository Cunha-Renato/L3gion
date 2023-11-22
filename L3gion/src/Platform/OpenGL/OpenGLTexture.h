#pragma once

#include "L3gion/Renderer/Texture.h"

#include <glad/glad.h>

namespace L3gion
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecs& specs);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual const TextureSpecs& getSpecs() const { return m_Specs; }

		virtual uint32_t getWidth() const override { return m_Width; }
		virtual uint32_t getHeight() const override { return m_Height; }
		virtual uint32_t getRendererID() const override { return m_RendererID; }

		virtual const std::string& getPath() const override { return m_Path; };

		void setData(void* data, uint32_t size) override;
		virtual void bind(uint32_t slot = 0) const override;

		virtual bool compare(const Texture& other) const override
		{
			return m_RendererID == other.getRendererID();
		}
	private:
		TextureSpecs m_Specs;

		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}

