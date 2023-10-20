#include "lgpch.h"
#include "Renderer.h"

namespace L3gion
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	Renderer::~Renderer()
	{
		if (m_SceneData)
			delete m_SceneData;
	}

	void Renderer::beginScene(OrthoCamera& camera)
	{
		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}
	void Renderer::endScene()
	{

	}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->setMat4("u_ViewProjection", m_SceneData->viewProjectionMatrix);
		shader->bind();

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}