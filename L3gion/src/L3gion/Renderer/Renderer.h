#pragma once

#include "RenderCommand.h"

#include "Shader.h"
#include "Camera.h"

namespace L3gion
{
	class Renderer
	{
	public:
		~Renderer();
		static void beginScene(OrthoCamera& camera);
		static void endScene();

		static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		
		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}