#include "EditorLayer.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <chrono>

#include "L3gion/Scene/Components.h"
#include "L3gion/Scene/SceneSerializer.h"
#include "L3gion/Utils/PlatformUtils.h"
#include "L3gion/Maths/Math.h"

#include "ImGuizmo.h"

namespace L3gion
{
	EditorLayer::EditorLayer(const std::string& name)
		: Layer(name)
	{
	}


	void EditorLayer::onAttach()
	{
		LG_PROFILE_FUNCTION();

		FrameBufferSpecs frameBufferSpecs;
		frameBufferSpecs.width = 1280;
		frameBufferSpecs.height = 720;
		m_FrameBuffer = FrameBuffer::create(frameBufferSpecs);

		// Entity
		m_ActiveScene = createRef<Scene>();
		m_SceneHierarchyPanel.setContext(m_ActiveScene);
		SceneSerializer serializer(m_ActiveScene);
	}

	void EditorLayer::onDetach()
	{
		LG_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep& ts)
	{
		LG_PROFILE_FUNCTION();

		m_Timesep = ts.getMiliSeconds();

		// Resize
		if (L3gion::FrameBufferSpecs spec = m_FrameBuffer->getSpecification();
			m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f) // zero sized framebuffer is invalid
		{
			if ((spec.width != m_ViewPortSize.x || spec.height != m_ViewPortSize.y))
				m_FrameBuffer->resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			
			m_ActiveScene->onViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		}


		// Render
		Renderer2D::resetStats();
		m_FrameBuffer->bind();
		RenderCommand::setClearColor(glm::vec4(0.15f, 0.1f, 0.1f, 1.0f));
		RenderCommand::clear();
		
		m_ActiveScene->onUptdate(ts);

		m_FrameBuffer->unbind();
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
		ImGui::Begin("My DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWindowSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWindowSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "CTRL+N"))
					newScene();
				if (ImGui::MenuItem("Open...", "CTRL+O"))
					openScene();
				if (ImGui::MenuItem("Save as...", "CTRL+SHIFT+S"))
					saveSceneAs();
				if (ImGui::MenuItem("Exit")) Application::get().close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.onImGuiRender();

//-------------------------------VIEWPORT--------------------------
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		{
			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHovered = ImGui::IsWindowHovered();

			Application::get().getImGui()->blockEvents(!m_ViewPortFocused && !m_ViewPortHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewPortSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t viewPort = m_FrameBuffer->getColorAttachmentRendererID();
			ImGui::Image((void*)(uint64_t)viewPort, viewportPanelSize, ImVec2{ 0,1 }, ImVec2{ 1, 0 });

		}

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera
			auto cameraEntity = m_ActiveScene->getPrimaryCameraEntity();
			const auto& camera = cameraEntity.getComponent<CameraComponent>().camera;
			const glm::mat4& cameraProjection = camera.getProjection();
			glm::mat4 cameraView = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());

			// Snapping
			bool snap = Input::isKeyPressed(LgKeys::LG_KEY_LEFT_CONTROL);
			float snapValue = 0.5f;
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			// Entity transform
			auto& tc = selectedEntity.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getTransform();

			ImGuizmo::Manipulate(
				glm::value_ptr(cameraView), 
				glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType,
				ImGuizmo::LOCAL, 
				glm::value_ptr(transform),
				nullptr,
				snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation{0.0f}, rotation{0.0f}, scale{1.0f};
				Math::decomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
//--------------------------------------------------------------------
		
		ImGui::Begin("Settings: ");
		ImGui::Text("Timestep: %.2fms \n%d fps", m_Timesep, (int)(1000 / m_Timesep));
		ImGui::Text("Profiling: %d", LG_PROFILE_IS_ACTIVE());
		ImGui::Separator();
		ImGui::Text("Renderer2D:");
		ImGui::Text("Draw Calls: %d", Renderer2D::getStats().drawCalls);
		ImGui::Text("Quad Count: %d", Renderer2D::getStats().quadCount);
		ImGui::Text("Vertex Count: %d", Renderer2D::getStats().getTotalVertexCount());
		ImGui::Text("Index Count: %d", Renderer2D::getStats().getTotalIndexCount());
		ImGui::Separator();
		ImGui::End();

		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e)
	{
		auto dispatcher = EventDispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::onKeyPressed));
	}

	bool EditorLayer::onKeyPressed(KeyPressedEvent& e)
	{
		if (e.getRepeatCount() > 0)
			return false;

		bool control = Input::isKeyPressed(LgKeys::LG_KEY_LEFT_CONTROL) || Input::isKeyPressed(LgKeys::LG_KEY_RIGHT_CONTROL);
		bool shift = Input::isKeyPressed(LgKeys::LG_KEY_LEFT_SHIFT) || Input::isKeyPressed(LgKeys::LG_KEY_RIGHT_SHIFT);

		switch (e.getKeyCode())
		{
			case LgKeys::LG_KEY_I:
			{
				if (LG_PROFILE_IS_ACTIVE())
				{
					LG_PROFILE_END_SESSION();
				}
				else
				{
					LG_PROFILE_BEGIN_SESSION("Runtime", "L3gionProfile_Runtime.json");
				}
				break;
			}
			case LgKeys::LG_KEY_N:
			{
				if (control)
					newScene();
				break;
			}
			case LgKeys::LG_KEY_O:
			{
				if (control)
					openScene();
				break;
			}
			case LgKeys::LG_KEY_S:
			{
				if (shift)
					saveSceneAs();
				break;
			}
			
			// Gizmos
			case LgKeys::LG_KEY_Q:
			{
				m_GizmoType = -1;
				break;
			}
			case LgKeys::LG_KEY_W:
			{
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case LgKeys::LG_KEY_E:
			{
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case LgKeys::LG_KEY_R:
			{
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
		default:
			break;
		}
	
		if (e.getKeyCode() == LgKeys::LG_KEY_I)
		{
			
		}

		return false;
	}

	void EditorLayer::newScene()
	{
		m_ActiveScene = createRef<Scene>();
		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}
	void EditorLayer::openScene()
	{
		std::string filepath = FileDialogs::openFile("L3gion Scene (*.lg)\0*.lg\0");
		if (!filepath.empty())
		{
			m_ActiveScene = createRef<Scene>();
			m_SceneHierarchyPanel.setContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.deserialize(filepath);
		}
	}
	void EditorLayer::saveSceneAs()
	{
		std::string filepath = FileDialogs::saveFile("L3gion Scene (*.lg)\0*.lg\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.serialize(filepath);
		}
	}
}