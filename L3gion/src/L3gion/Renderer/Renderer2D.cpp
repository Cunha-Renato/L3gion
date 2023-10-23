#include "lgpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace L3gion
{
	struct Renderer2DSorage
	{
		ref<VertexArray> vertexArray;
		ref<Shader> flatColorShader;
	};
	
	static Renderer2DSorage* s_Data;

	void Renderer2D::init()
	{
		s_Data = new Renderer2DSorage();

		s_Data->vertexArray = VertexArray::create();

		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		auto vertexBuffer = VertexBuffer::create(vertices, sizeof(vertices));

		BufferLayout layout =
		{
			{ShaderDataType::Float3, "a_Position"}
		};

		vertexBuffer->setLayout(layout);
		s_Data->vertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		auto indexBuffer = IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
		s_Data->vertexArray->setIndexBuffer(indexBuffer);

		s_Data->flatColorShader = Shader::create("assets/Shaders/FlatColor.glsl");
	}
	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::beginScene(const OrthoCamera& camera)
	{
		s_Data->flatColorShader->bind();
		s_Data->flatColorShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
	}
	void Renderer2D::endScene()
	{

	}

	// Primitives
	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color)
	{
		drawQuad(glm::vec3(position, 0.0f), size, color);
	}
	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color)
	{
		s_Data->flatColorShader->bind();
		s_Data->flatColorShader->setFloat4("u_Color", color);

		glm::mat4 transfom = glm::translate(glm::mat4(1.0f), position) * 
			glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		s_Data->flatColorShader->setMat4("u_Transform", transfom);

		s_Data->vertexArray->bind();
		RenderCommand::drawIndexed(s_Data->vertexArray);
	}
}