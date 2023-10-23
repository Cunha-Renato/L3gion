#include "Sandbox2D.h"

#include <glm/gtc/type_ptr.hpp>

#include "ImGui/imgui.h"

Sandbox2D::Sandbox2D(const std::string& name)
	: Layer(name), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::onAttach()
{
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
		{L3gion::ShaderDataType::Float3, "a_Position"}
	};

	vertexBuffer->setLayout(layout);
	m_VertexArray->addVertexBuffer(vertexBuffer);

	uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

	auto indexBuffer = L3gion::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->setIndexBuffer(indexBuffer);

	auto shader = m_ShaderLib.load("assets/Shaders/FlatColor.glsl");
}

void Sandbox2D::onDetach()
{}

void Sandbox2D::onUpdate(L3gion::Timestep& ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	L3gion::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	L3gion::RenderCommand::clear();

	auto shader = m_ShaderLib.get("FlatColor");

	L3gion::Renderer2D::beginScene(m_CameraController.getCamera());
	{

		L3gion::Renderer2D::drawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_Color);
		L3gion::Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });

	}
	L3gion::Renderer2D::endScene();
}
void Sandbox2D::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Color", glm::value_ptr(m_Color));
	ImGui::End();
}
void Sandbox2D::onEvent(L3gion::Event& e)
{
	m_CameraController.onEvent(e);
}