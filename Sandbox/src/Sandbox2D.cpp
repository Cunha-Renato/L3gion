#include "Sandbox2D.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <chrono>

Sandbox2D::Sandbox2D(const std::string& name)
	: Layer(name), m_CameraController(1280.0f / 720.0f)
{
}

static L3gion::ref<L3gion::Texture2D> l3gionTexture;
static L3gion::ref<L3gion::Texture2D> chernoTexture;

void Sandbox2D::onAttach()
{
	LG_PROFILE_FUNCTION();
	m_Texture = L3gion::Texture2D::create("../Sandbox/assets/textures/Checkerboard.png");
	l3gionTexture = L3gion::Texture2D::create("../Sandbox/assets/textures/L3gion_Engine.png");
	chernoTexture = L3gion::Texture2D::create("../Sandbox/assets/textures/ChernoLogo.png");
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
		L3gion::RenderCommand::setClearColor(glm::vec4(1.0f, 0.2f, 0.6f, 1.0f));
		L3gion::RenderCommand::clear();
	}

	{
		LG_PROFILE_SCOPE("Renderer drawQuad: ");
		L3gion::Renderer2D::beginScene(m_CameraController.getCamera());

		L3gion::Renderer2D::drawQuad({
			.position = { 0.0f, 0.0f, -0.1f },
			.size = { 200.0f, 200.0f },
			.color = m_Color
			});

		L3gion::QuadSpecs quad;

		for (int x = 0; x < 200; x++)
		{
			for (int y = 0; y < 200; y++)
			{
				quad.position = {x, y, 0.0f};
				
				if (y % 2 == 0)
				{
					if(x%2 == 0)
						quad.texture = l3gionTexture;
					else
						quad.texture = m_Texture;
					
				}
				else
				{
					if (x % 2 == 0)
						quad.texture = m_Texture;
					else
						quad.texture = l3gionTexture;
				}

				L3gion::Renderer2D::drawQuad(quad);
			}
		}

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