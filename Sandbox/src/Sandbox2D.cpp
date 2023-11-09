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
	m_CameraController.setZoomLevel(5.0f);
	m_Checkerboard = L3gion::SubTexture2D::create("../Sandbox/assets/textures/Checkerboard.png");
	m_SpriteSheetTexture = L3gion::Texture2D::create("../Sandbox/assets/textures/game/Game_SpriteSheet.png");
	m_Tree = L3gion::SubTexture2D::create(m_SpriteSheetTexture, { 2, 1 }, { 128, 128 }, {1, 2});

}

void Sandbox2D::onDetach()
{
	LG_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(L3gion::Timestep& ts)
{
	LG_PROFILE_FUNCTION();

	m_Timesep = ts;
	m_CameraController.onUpdate(ts);

	L3gion::Renderer2D::resetStats();
	// Render
	{
		LG_PROFILE_SCOPE("Renderer Prep: ");
		L3gion::RenderCommand::setClearColor(glm::vec4(1.0f, 0.2f, 0.6f, 1.0f));
		L3gion::RenderCommand::clear();
	}

	{
		LG_PROFILE_SCOPE("Renderer drawQuads: ");
		L3gion::Renderer2D::beginScene(m_CameraController.getCamera()., m_CameraController.getCamera().getViewProjectionMatrix());

		L3gion::Renderer2D::drawQuad({
			.position = { 0.0f, 0.0f, -0.1f },
			.size = { 10.0f, 10.0f },
			.subTexture = m_Checkerboard
			});

		L3gion::Renderer2D::drawQuad({
			.position = { 0.0f, 0.0f, 0.0f },
			.size = { 1.0f, 2.0f },
			.subTexture = m_Tree
		});

		L3gion::Renderer2D::drawQuad({
			.position = { -3.2f, 0.0f, 0.0f },
			.size = { 3.0f, 2.0f },
			.color = m_Color
			});

		L3gion::Renderer2D::endScene();
	}
}

void Sandbox2D::onImGuiRender()
{
	LG_PROFILE_FUNCTION();

	ImGui::Begin("Performance: ");
	ImGui::Text("Timestep: %.4fms (%d fps)", m_Timesep, (int)(1 / m_Timesep));
	ImGui::Text("Profiling: %d", LG_PROFILE_IS_ACTIVE());
	ImGui::Text("\nRenderer2D:");
	ImGui::Text("Draw Calls: %d", L3gion::Renderer2D::getStats().drawCalls);
	ImGui::Text("Quad Count: %d", L3gion::Renderer2D::getStats().quadCount);
	ImGui::Text("Vertex Count: %d", L3gion::Renderer2D::getStats().getTotalVertexCount());
	ImGui::Text("Index Count: %d", L3gion::Renderer2D::getStats().getTotalIndexCount());
	ImGui::Text("\nRender Settings");
	ImGui::ColorEdit4("Color", glm::value_ptr(m_Color));
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
	if (L3gion::Input::isKeyPressed(LgKeys::LG_KEY_I))
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