#include "Sandbox2D.h"

#include <glm/gtc/type_ptr.hpp>

#include <ImGui/imgui.h>

Sandbox2D::Sandbox2D(const std::string& name)
	: Layer(name), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::onAttach()
{
	m_Texture = L3gion::Texture2D::create("assets/textures/checkerboard.png");
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

	L3gion::Renderer2D::beginScene(m_CameraController.getCamera());
	{
		L3gion::Renderer2D::drawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_Color);
		L3gion::Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		L3gion::Renderer2D::drawQuad({ 0.0f, 0.0f, -0.1f}, { 10.0f, 10.0f }, m_Texture);
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