#include "lgpch.h"
#include "L3gion/Renderer/Renderer2D.h"

#include "L3gion/Renderer/VertexArray.h"
#include "L3gion/Renderer/Shader.h"
#include "L3gion/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace L3gion
{
	struct Renderer2DSorage
	{
		ref<VertexArray> vertexArray;
		ref<Shader> textureShader;
		ref<Texture2D> whiteTexture;
	};
	
	static Renderer2DSorage* s_Data;

	void Renderer2D::init()
	{
		s_Data = new Renderer2DSorage();

		s_Data->vertexArray = VertexArray::create();

		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		auto vertexBuffer = VertexBuffer::create(vertices, sizeof(vertices));

		BufferLayout layout =
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TextCoords"}
		};

		vertexBuffer->setLayout(layout);
		s_Data->vertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		auto indexBuffer = IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
		s_Data->vertexArray->setIndexBuffer(indexBuffer);

		s_Data->whiteTexture = Texture2D::create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		s_Data->textureShader = Shader::create("assets/Shaders/Texture.glsl");
		s_Data->textureShader->bind();
		s_Data->textureShader->setInt("u_Texture", 0);
	}
	void Renderer2D::shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::beginScene(const OrthoCamera& camera)
	{
		s_Data->textureShader->bind();
		s_Data->textureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
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
		s_Data->textureShader->setFloat4("u_Color", color);
		s_Data->whiteTexture->bind();

		glm::mat4 transfom = glm::translate(glm::mat4(1.0f), position) * 
			glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		s_Data->textureShader->setMat4("u_Transform", transfom);

		s_Data->vertexArray->bind();
		RenderCommand::drawIndexed(s_Data->vertexArray);
	}
	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2 size, const ref<Texture2D> texture)
	{
		drawQuad(glm::vec3(position, 0.0f), size, texture);
	}
	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2 size, const ref<Texture2D> texture)
	{
		s_Data->textureShader->setFloat4("u_Color", glm::vec4(1.0f));
		texture->bind();

		glm::mat4 transfom = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
		s_Data->textureShader->setMat4("u_Transform", transfom);

		s_Data->vertexArray->bind();
		RenderCommand::drawIndexed(s_Data->vertexArray);
	}
}