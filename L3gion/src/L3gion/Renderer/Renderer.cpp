#include "lgpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace L3gion
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	Renderer::~Renderer()
	{
		if (m_SceneData)
			delete m_SceneData;
	}

	void Renderer::init()
	{
		RenderCommand::init();
	}

	void Renderer::beginScene(OrthoCamera& camera)
	{
		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}
	void Renderer::endScene()
	{

	}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->setMat4("u_ViewProjection", m_SceneData->viewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->setMat4("u_Transform", transform);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}