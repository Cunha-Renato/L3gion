#pragma once

#include "L3gion/Renderer/Camera.h"
#include "L3gion/Renderer/Texture.h"

namespace L3gion
{
	struct QuadSpecifications
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec2 size = glm::vec2(1.0f);
		float rotation = 0.0f;
		glm::vec4 color = glm::vec4(1.0f);
		ref<Texture2D> texture = nullptr;
		float tiling = 1.0f;
	};

	class Renderer2D
	{
	public:
		static void init();
		static void shutdown();

		static void beginScene(const OrthoCamera& camera);
		static void endScene();

		// Primitives
		static void drawQuad(const QuadSpecifications& specs);
		static void drawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color);
		static void drawQuad(const glm::vec2& position, const glm::vec2 size, ref<Texture2D> texture);
		static void drawQuad(const glm::vec3& position, const glm::vec2 size, const ref<Texture2D> texture);
	};
}
