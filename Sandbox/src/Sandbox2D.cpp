#include "Sandbox2D.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <chrono>

Sandbox2D::Sandbox2D(const std::string& name)
	: Layer(name), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::onAttach()
{
	LG_PROFILE_FUNCTION();
	m_Texture = L3gion::Texture2D::create("assets/textures/checkerboard.png");
}

void Sandbox2D::onDetach()
{
	LG_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(L3gion::Timestep& ts)
{
	LG_PROFILE_FUNCTION();

	m_FpsCounter = (int)(1 / ts.getSeconds());

	m_CameraController.onUpdate(ts);

	// Render
	{
		LG_PROFILE_SCOPE("RendererCommands: ");
		L3gion::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		L3gion::RenderCommand::clear();
	}

	{
		LG_PROFILE_SCOPE("Renderer drawQuad: ");
		L3gion::Renderer2D::beginScene(m_CameraController.getCamera());

		L3gion::Renderer2D::drawQuad({
			.position = { -1.0f, 0.0f, 0.0f},
			.size = { 0.8f, 0.8f },
			.color = m_Color
			});

		L3gion::Renderer2D::drawQuad({
			.position = { 0.5f, -0.5f, 0.0f }, 
			.size = { 0.5f, 0.75f }, 
			.color = { 0.2f, 0.3f, 0.8f, 1.0f }
			});

		L3gion::Renderer2D::drawQuad({
			.position = { 0.0f, 0.0f, -0.1f}, 
			.size = { 10.0f, 10.0f },
			.color = { 0.2, 0.5, 0.6, 1.0f },
			.texture = m_Texture,
			});

		L3gion::Renderer2D::endScene();
	}
}
void Sandbox2D::onImGuiRender()
{
	LG_PROFILE_FUNCTION();

	ImGui::Begin("Render Settings");
	ImGui::ColorEdit4("Color", glm::value_ptr(m_Color));
	ImGui::End();

	char text[30];
	ImGui::Begin("Performance");
	strcpy_s(text, "%d fps");
	ImGui::Text(text, m_FpsCounter);

	strcpy_s(text, "Profiling: ");
	strcat_s(text, " %d");
	ImGui::Text(text, LG_PROFILE_IS_ACTIVE());
	ImGui::End();
}

void Sandbox2D::onEvent(L3gion::Event& e)
{
	m_CameraController.onEvent(e);

	auto dispatcher = L3gion::EventDispatcher(e);
	dispatcher.dispatch<L3gion::KeyPressedEvent>(BIND_EVENT_FN(Sandbox2D::onKeyPressed));
}

bool Sandbox2D::onKeyPressed(L3gion::Event& e)
{
	if (L3gion::Input::isKeyPressed(LG_KEY_I))
	{
		if (LG_PROFILE_IS_ACTIVE())
		{
			LG_PROFILE_END_SESSION();
		}
		else
		{
			LG_PROFILE_BEGIN_SESSION("Runtime", "L3gionProfile_Runtime.json");
		}
	}

	return false;
}