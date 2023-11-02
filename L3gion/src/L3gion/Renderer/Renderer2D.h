#pragma once

#include "L3gion/Renderer/Camera.h"
#include "L3gion/Renderer/Texture.h"
#include "L3gion/Renderer/SubTexture.h"
#include "L3gion/Renderer/EditorCamera.h"

namespace L3gion
{
	class Renderer2D
	{
	public:
		static void init();
		static void shutdown();

		static void beginScene(const Camera& camera, const glm::mat4& transform);
		static void beginScene(const EditorCamera& editorCamera);
		static void beginScene(const OrthoCamera& camera);
		static void endScene();
		static void flush();

		// Primitives
		struct QuadSpecs
		{
			glm::mat4 transform = glm::mat4(1.0f);
			glm::vec4 color = glm::vec4(1.0f);
			int id = -1; // Editor Only
			ref<SubTexture2D> subTexture = nullptr;
			float tiling = 1.0f;
		};

		static void drawQuad(const QuadSpecs& specs);

		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			uint32_t getTotalVertexCount() const { return quadCount * 4; }
			uint32_t getTotalIndexCount() const { return quadCount * 6; }
		};

		static Statistics getStats();
		static void resetStats();

	private:
		static void flushAndReset();
	};
}