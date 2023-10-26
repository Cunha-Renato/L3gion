#pragma once

#include "L3gion/Renderer/Camera.h"
#include "L3gion/Renderer/Texture.h"

namespace L3gion
{
	struct QuadSpecs
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec2 size = glm::vec2(1.0f);
		float angle = 0.0f; // In radians
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
		static void flush();

		// Primitives
		static void drawQuad(const QuadSpecs& specs);
	};
}