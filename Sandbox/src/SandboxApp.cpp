#include <L3gion.h>
#include <L3gion/Core/EntryPoint.h>

#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "L3gion/Renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>


class ExampleLayer : public L3gion::Layer
{
public:
	ExampleLayer(std::string name)
		: m_CameraController(1280.0f / 720.0f, true),Layer(name), m_Color(0.0f)
	{
		// OpenGL buffers
		m_VertexArray = L3gion::VertexArray::create();

		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		auto vertexBuffer = L3gion::VertexBuffer::create(vertices, sizeof(vertices));

		L3gion::BufferLayout layout =
		{
			{L3gion::ShaderDataType::Float3, "a_Position"},
			{L3gion::ShaderDataType::Float2, "a_TextCoord"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		auto indexBuffer = L3gion::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->setIndexBuffer(indexBuffer);

		auto shader = m_ShaderLib.load("assets/Shaders/Texture.glsl");

		m_Texture = L3gion::Texture2D::create("assets/textures/checkerboard.png");
		m_TextureTransparent = L3gion::Texture2D::create("assets/textures/L3gion_Engine.png");

		shader->bind();
		shader->setInt("u_Texture", 0);
	}

	void onUpdate(L3gion::Timestep& ts) override
	{
		// Update
		m_CameraController.onUpdate(ts);

		// Render
		L3gion::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		L3gion::RenderCommand::clear();

		auto shader = m_ShaderLib.get("Texture");

		L3gion::Renderer::beginScene(m_CameraController.getCamera());
		{
			m_Texture->bind();
			L3gion::Renderer::submit(shader, m_VertexArray);
			m_TextureTransparent->bind();
			L3gion::Renderer::submit(shader, m_VertexArray);
		}
		L3gion::Renderer::endScene();
	}

	void onEvent(L3gion::Event& e) override
	{
		m_CameraController.onEvent(e);
	}

	virtual void onImGuiRender() override
	{
		//ImGui::Begin("Settings");
		//
		//ImGui::ColorEdit3("Color", glm::value_ptr(m_Color));
		//
		//ImGui::End();
	}

private:
	L3gion::ShaderLib m_ShaderLib;
	L3gion::ref<L3gion::VertexArray> m_VertexArray;
	L3gion::ref<L3gion::Texture2D> m_Texture, m_TextureTransparent;

	glm::vec4 m_Color;

	L3gion::OrthoCameraController m_CameraController;
};

//------------------------- SANDBOX_APPLICATION ---------------------

class Sandbox : public L3gion::Application
{
public:
	Sandbox()
	{
		pushLayer(new Sandbox2D("Sandbox 2D Layer"));
		setVsync(false);
	}

	~Sandbox()
	{
	}
};

L3gion::Application* L3gion::createApplication()
{
	auto app = new Sandbox();
	return app;
}