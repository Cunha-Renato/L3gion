#include "lgpch.h"
#include "L3gion/Renderer/SubTexture.h"

namespace L3gion
{
	SubTexture2D::SubTexture2D(const ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		: m_Texture(texture)
	{
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}

	ref<SubTexture2D> SubTexture2D::create(const std::string& path)
	{
		auto texture = Texture2D::create(path);

		return SubTexture2D::create(texture);
	}
	ref<SubTexture2D> SubTexture2D::create(const ref<Texture2D>& texture)
	{
		glm::vec2 min = { 0.0f, 0.0f };
		glm::vec2 max = { 1.0f, 1.0f };

		return createRef<SubTexture2D>(texture, min, max);
	}
	ref<SubTexture2D> SubTexture2D::create(const TextureSpecs& specs)
	{
		glm::vec2 min = { 0.0f, 0.0f };
		glm::vec2 max = { 1.0f, 1.0f };

		auto texture = Texture2D::create(specs);
		return createRef<SubTexture2D>(texture, min, max);
	}

	ref<SubTexture2D> SubTexture2D::create(const ref<Texture2D>& texture, const glm::vec2& coords, glm::vec2 cellSize, const glm::vec2& spriteSize)
	{
		glm::vec2 min = { (coords.x * cellSize.x) / texture->getWidth(), (coords.y * cellSize.y) / texture->getHeight() };
		glm::vec2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texture->getWidth(),((coords.y + spriteSize.y) * cellSize.y) / texture->getHeight() };

		return createRef<SubTexture2D>(texture, min, max);
	}
}