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
		static void endScene();
		static void flush();

		// Primitives
		struct QuadSpecs
		{
			int id = -1; // Editor Only
			
			glm::mat4 transform = glm::mat4(1.0f);
			glm::vec4 color = glm::vec4(1.0f);
			ref<SubTexture2D> subTexture = nullptr;
			float tiling = 1.0f;

			float distanceFromCamera = 0.0f;
		};

		struct CircleSpecs
		{
			int id = -1; // Editor Only

			glm::mat4 transform = glm::mat4(1.0f);
			glm::vec4 color = glm::vec4(1.0f);
			float thickness = 0.1f;
			float smoothness = 0.005f;

			float distanceFromCamera = 0.0f;
		};

		struct LineSpecs
		{
			int id = -1; // Editor Only

			glm::vec3 p0 = glm::vec3(0.0f);
			glm::vec3 p1 = glm::vec3(0.0f);
			glm::vec4 color = glm::vec4(1.0f);
		};

		struct RectSpecs
		{
			int id = -1; // Editor Only

			bool useTransform = false;

			glm::mat4 transform = glm::mat4(0.0f);
			glm::vec3 position = glm::vec3(0.0f);
			glm::vec2 size = glm::vec2(1.0f);
			glm::vec4 color = glm::vec4(1.0f);
		};

		static void draw(const QuadSpecs& specs) { drawQuad(specs); }
		static void draw(const CircleSpecs& specs) { drawCircle(specs); }
		static void draw(const LineSpecs& specs) { drawLine(specs); }
		static void draw(const RectSpecs& specs) { drawRect(specs); }

		static void drawQuad(const QuadSpecs& specs);
		static void drawCircle(const CircleSpecs& specs);
		static void drawLine(const LineSpecs& specs);
		static void drawRect(const RectSpecs& specs);

		static float getLineThickness();
		static void setLineThickness(float thickness);

		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;
			uint32_t circleCount = 0;

			uint32_t getTotalVertexCount() const { return (quadCount + circleCount) * 4; }
			uint32_t getTotalIndexCount() const { return (quadCount + circleCount) * 6; }
		};

		static Statistics getStats();
		static void resetStats();

	private:
		static void flushAndReset();
		static void fillQuadBuffer();
		static void fillCircleBuffer();
	};
}