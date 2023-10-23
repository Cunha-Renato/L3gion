#include "lgpch.h"

#include "L3gion/Renderer/Renderer.h"
#include "L3gion/Renderer/Renderer2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace L3gion
{
	scope<Renderer::SceneData> Renderer::m_SceneData = createScope<Renderer::SceneData>();

	Renderer::~Renderer()
	{
		shutdown();
	}

	void Renderer::shutdown()
	{
		Renderer2D::shutdown();
	}

	void Renderer::init()
	{
		RenderCommand::init();
		Renderer2D::init();
	}
	
	void Renderer::onWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::setViewPort(0, 0, width, height);
	}


	void Renderer::beginScene(OrthoCamera& camera)
	{
		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}
	void Renderer::endScene()
	{

	}

	void Renderer::submit(const ref<Shader>& shader, const ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->bind();
		shader->setMat4("u_ViewProjection", m_SceneData->viewProjectionMatrix);
		shader->setMat4("u_Transform", transform);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}