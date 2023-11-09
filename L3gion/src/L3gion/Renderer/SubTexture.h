#pragma once

#include "L3gion/Renderer/Texture.h"
#include <glm/glm.hpp>

namespace L3gion
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);
		
		const ref<Texture2D> getTexture() const { return m_Texture; }
		const glm::vec2* getTexCoords() const { return m_TexCoords; }
		const uint32_t getID() const { return m_Texture->getRendererID(); }
		const std::string& getPath() const { return m_Texture->getPath(); }

		static ref<SubTexture2D> create(const std::string& path);
		static ref<SubTexture2D> create(const ref<Texture2D>& texture);
		// Coords is the offset in units (spriteSize) where every sprite has width and height of cellSize.
		static ref<SubTexture2D> create(const ref<Texture2D>& texture, const glm::vec2& coords, glm::vec2 cellSize, const glm::vec2& spriteSize = { 1.0f, 1.0f });

	private:
		ref<Texture2D> m_Texture;
		glm::vec2 m_CellSize;
		glm::vec2 m_TexCoords[4];
	};
}

