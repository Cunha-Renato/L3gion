#pragma once

#include "Camera.h"
#include "Texture.h"

namespace L3gion
{
	class Renderer2D
	{
	public:
		static void init();
		static void Shutdown();

		static void beginScene(const OrthoCamera& camera);
		static void endScene();

		// Primitives
		static void drawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color);
		static void drawQuad(const glm::vec2& position, const glm::vec2 size, ref<Texture2D> texture);
		static void drawQuad(const glm::vec3& position, const glm::vec2 size, const ref<Texture2D> texture);
	};
}
