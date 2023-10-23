#pragma once

#include "L3gion/Renderer/RenderCommand.h"

#include "L3gion/Renderer/Shader.h"
#include "L3gion/Renderer/Camera.h"

namespace L3gion
{
	class Renderer
	{
	public:
		~Renderer();

		static void init();
		static void shutdown();
		static void onWindowResize(uint32_t width, uint32_t height);

		static void beginScene(OrthoCamera& camera);
		static void endScene();

		static void submit(const ref<Shader>& shader, const ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		
		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		static scope<SceneData> m_SceneData;
	};
}