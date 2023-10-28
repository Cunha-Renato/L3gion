#include "EditorLayer.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <chrono>

namespace L3gion
{
	EditorLayer::EditorLayer(const std::string& name)
		: Layer(name), m_CameraController(1280.0f / 720.0f)
	{
	}


	void EditorLayer::onAttach()
	{
		LG_PROFILE_FUNCTION();
		m_CameraController.setZoomLevel(5.0f);
		m_Checkerboard = SubTexture2D::create("../Sandbox/assets/textures/Checkerboard.png");
		m_SpriteSheetTexture = Texture2D::create("../Sandbox/assets/textures/game/Game_SpriteSheet.png");
		m_Tree = SubTexture2D::create(m_SpriteSheetTexture, { 2, 1 }, { 128, 128 }, { 1, 2 });

		FrameBufferSpecs frameBufferSpecs;
		frameBufferSpecs.width = 1280;
		frameBufferSpecs.height = 720;
		m_FrameBuffer = FrameBuffer::create(frameBufferSpecs);
	}

	void EditorLayer::onDetach()
	{
		LG_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep& ts)
	{
		LG_PROFILE_FUNCTION();

		m_Timesep = ts;

		// Resize
		if (L3gion::FrameBufferSpecs spec = m_FrameBuffer->getSpecification();
			m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != m_ViewPortSize.x || spec.height != m_ViewPortSize.y))
		{
			m_FrameBuffer->resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_CameraController.resize(m_ViewPortSize.x, m_ViewPortSize.y);
		}

		if (m_ViewPortFocused)
			m_CameraController.onUpdate(ts);

		Renderer2D::resetStats();
		// Render
		{
			LG_PROFILE_SCOPE("Renderer Prep: ");
			m_FrameBuffer->bind();
			RenderCommand::setClearColor(glm::vec4(1.0f, 0.2f, 0.6f, 1.0f));
			RenderCommand::clear();
		}

		{
			LG_PROFILE_SCOPE("Renderer drawQuads: ");
			Renderer2D::beginScene(m_CameraController.getCamera());

			Renderer2D::drawQuad({
				.position = { 0.0f, 0.0f, -0.1f },
				.size = { 10.0f, 10.0f },
				.subTexture = m_Checkerboard
				});

			Renderer2D::drawQuad({
				.position = { 0.0f, 0.0f, 0.0f },
				.size = { 1.0f, 2.0f },
				.subTexture = m_Tree
				});

			Renderer2D::drawQuad({
				.position = { -3.2f, 0.0f, 0.0f },
				.size = { 3.0f, 2.0f },
				.color = m_Color
				});

			Renderer2D::endScene();
			m_FrameBuffer->unbind();
		}
	}

	void EditorLayer::onImGuiRender()
	{
		LG_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) Application::get().close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Begin("Performance: ");
		ImGui::Text("Timestep: %.4fms (%d fps)", m_Timesep, (int)(1 / m_Timesep));
		ImGui::Text("Profiling: %d", LG_PROFILE_IS_ACTIVE());
		ImGui::Text("\nRenderer2D:");
		ImGui::Text("Draw Calls: %d", Renderer2D::getStats().drawCalls);
		ImGui::Text("Quad Count: %d", Renderer2D::getStats().quadCount);
		ImGui::Text("Vertex Count: %d", Renderer2D::getStats().getTotalVertexCount());
		ImGui::Text("Index Count: %d", Renderer2D::getStats().getTotalIndexCount());
		ImGui::Text("\nRender Settings");
		ImGui::ColorEdit4("Color", glm::value_ptr(m_Color));
		ImGui::End();

//-------------------------------VIEWPORT--------------------------
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		{
			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHovered = ImGui::IsWindowHovered();

			Application::get().getImGui()->blockEvents(!m_ViewPortFocused || !m_ViewPortHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewPortSize = { viewportPanelSize.x, viewportPanelSize.y };
	
			uint32_t viewPort = m_FrameBuffer->getColorAttachmentRendererID();
			ImGui::Image((void*)viewPort, viewportPanelSize, ImVec2{0,1}, ImVec2{1, 0});

		}
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();
		
	}

	void EditorLayer::onEvent(Event& e)
	{
		m_CameraController.onEvent(e);

		auto dispatcher = EventDispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::onKeyPressed));
	}

	bool EditorLayer::onKeyPressed(Event& e)
	{
		if (Input::isKeyPressed(LG_KEY_I))
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
}