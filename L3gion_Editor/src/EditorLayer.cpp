#include "EditorLayer.h"

#include "L3gion/Core/Timer.h"
#include "L3gion/Scripting/ScriptEngine.h"
#include "L3gion/Scene/Components.h"
#include "L3gion/Scene/SceneSerializer.h"
#include "L3gion/Utils/PlatformUtils.h"
#include "L3gion/Maths/Math.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <chrono>
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

		m_PlayIcon = SubTexture2D::create("resources/icons/PlayButtonIcon.png");
		m_StopIcon = SubTexture2D::create("resources/icons/StopButtonIcon.png");
		m_SimulateIcon = SubTexture2D::create("resources/icons/PlaySimulationButtonIcon.png");

		FramebufferSpecs frameBufferSpecs;
		frameBufferSpecs.attachments = { 
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::Depth 
		};
		frameBufferSpecs.width = 1280;
		frameBufferSpecs.height = 720;
		m_FrameBuffer = Framebuffer::create(frameBufferSpecs);

		// Entity
		m_ActiveScene = createRef<Scene>();
		m_EditorScene = m_ActiveScene;

		auto commandLineArgs = Application::get().getSpecs().commandArgs;
		if (commandLineArgs.count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.deserialize(sceneFilePath);
		}
		
		m_EditorCamera = EditorCamera(30.0f, 16.0f/9.0f, 0.01f, 1000.0f);

		m_SceneHierarchyPanel.setContext(m_ActiveScene);

		if (!m_ContentBrowserPanel && !Project::getActive())
		{
			while(!openProject());
			m_ContentBrowserPanel = createRef<ContentBrowserPanel>();
		}
	}
	void EditorLayer::onDetach()
	{
		LG_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep& ts)
	{
		LG_PROFILE_FUNCTION();

		m_Timestep = ts.getMiliSeconds();

		// Resize
		if (L3gion::FramebufferSpecs spec = m_FrameBuffer->getSpecification();
			m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f) // zero sized framebuffer is invalid
		{
			if ((spec.width != m_ViewPortSize.x || spec.height != m_ViewPortSize.y))
				m_FrameBuffer->resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			
			m_EditorCamera.setViewportSize(m_ViewPortSize.x, m_ViewPortSize.y);
			m_ActiveScene->onViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		}

		// Render
		Renderer2D::resetStats();
		m_FrameBuffer->bind();
		RenderCommand::setClearColor(glm::vec4(0.15f, 0.1f, 0.1f, 1.0f));
		RenderCommand::clear();

		// Clear entityID attachmet to -1
		m_FrameBuffer->clearTexColor(1, -1);
		
		// Update Scene

		switch (m_SceneState)
		{
			case L3gion::EditorLayer::SceneState::Edit:
			{	
				if (m_ViewPortHovered)
					m_EditorCamera.onUpdate(ts);
				m_ActiveScene->onUptdateEditor(ts, m_EditorCamera);

				break;
			}
			case L3gion::EditorLayer::SceneState::Simulate:
			{
				if (m_ViewPortHovered)
					m_EditorCamera.onUpdate(ts);

				m_ActiveScene->onUpdateSimulation(ts, m_EditorCamera);

				break;
			}
			case L3gion::EditorLayer::SceneState::Play:
			{
				m_ActiveScene->onUptdateRuntime(ts);

				break;
			}
		}
		
		onOverlayRender();

		m_FrameBuffer->unbind();
	}

	void EditorLayer::onOverlayRender()
	{
		LG_PROFILE_FUNCTION();
		
		auto view = m_ActiveScene->getAllEntitiesWith<Collider2DComponent, TransformComponent>();
		
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->getPrimaryCameraEntity();
			if (!camera) return;
			Renderer2D::beginScene(camera.getComponent<CameraComponent>().camera, camera.getComponent<TransformComponent>().getTransform());
		}
		else
			Renderer2D::beginScene(m_EditorCamera);
		
		// Selected Component outline
		auto selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();

		if (selectedEntity)
		{
			auto selectedTransform = selectedEntity.getComponent<TransformComponent>().getTransform();
			selectedTransform[3].z += 0.009f;
		
			Renderer2D::drawRect({
				.useTransform = true,
				.transform = selectedTransform,
				.color = {1.0f, 0.4f, 0.2f, 1.0f},
			});
		}

		// Colliders
		if (m_ShowColliders)
		{
			for (auto entity : view)
			{
				auto [tc, collider2D] = view.get<TransformComponent, Collider2DComponent>(entity);

				float angle = collider2D.angle + std::atan2(collider2D.offset.y, collider2D.offset.x);
				float radius = glm::distance(glm::vec2(tc.translation.x, tc.translation.y), glm::vec2(collider2D.offset.x + tc.translation.x, collider2D.offset.y + tc.translation.y));
				glm::vec3 translation = glm::vec3(tc.translation.x + radius * cos(angle), tc.translation.y + radius * sin(angle), 0.005f);

				if (collider2D.type == Collider2DComponent::Type::Box)
				{
					glm::vec3 scale = tc.scale * glm::vec3(collider2D.radius * 2.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::rotate(glm::mat4(1.0f), tc.rotation.z, { 0, 0, 1 }) * glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::drawRect({
						.useTransform = true,
						.transform = transform,
						.color = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f)
						});
				}
				else if (collider2D.type == Collider2DComponent::Type::Circle)
				{
					glm::vec3 scale = tc.scale * glm::vec3(collider2D.radius * 2.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::drawCircle({
						.transform = transform,
						.color = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f),
						.thickness = 0.05f
						});
				}
			}
		}

		Renderer2D::endScene();
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
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav;
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
		style.WindowMinSize.x = 355.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWindowSizeX;

// End of ImGui dockspace code -------------------------------------------------------

		m_SceneHierarchyPanel.onImGuiRender();

		if (m_ContentBrowserPanel)
			m_ContentBrowserPanel->onImGuiRender();

		UI_Viewport([&]()
		{
			UI_Gizmos();
		});

		UI_Stats();
		UI_Settings();
		UI_Toolbar();
		UI_Header();

		ImGui::End();
	}

	template<typename Func>
	void EditorLayer::UI_Viewport(Func func)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoNav);
		{
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHovered = ImGui::IsWindowHovered();

			Application::get().getImGui()->blockKeyEvents(!m_ViewPortFocused);
			Application::get().getImGui()->blockMouseEvents(!m_ViewPortHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewPortSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t viewPort = m_FrameBuffer->getColorAttachmentRendererID(0);
			ImGui::Image((void*)(uint64_t)viewPort, viewportPanelSize, ImVec2{ 0,1 }, ImVec2{ 1, 0 });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					std::filesystem::path path = *(std::filesystem::path*)payload->Data;
					
					openScene(path);
				}

				ImGui::EndDragDropTarget();
			}
		}

		func();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::UI_Gizmos()
	{
		Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// EditorCamera
			glm::mat4 cameraProjection = glm::mat4(1.0f);
			glm::mat4 cameraView;
			
			if (m_SceneState != SceneState::Play)
			{
				cameraProjection = m_EditorCamera.getProjection();
				cameraView = m_EditorCamera.getViewMatrix();
			}
			else if (m_SceneState == SceneState::Play)
			{
				//auto [transform, camera] = m_ActiveScene->getPrimaryCameraEntity().getComponent<TransformComponent, CameraComponent>();
				//cameraProjection = camera.camera.getProjection();
				//cameraView = transform.getTransform();
			}

			// Snapping
			bool snap = Input::isKeyPressed(LgKey::LEFT_CONTROL);
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

			if (ImGuizmo::IsUsing() && !Input::isKeyPressed(LgKey::LEFT_ALT))
			{
				glm::vec3 translation{ 0.0f }, rotation{ 0.0f }, scale{ 1.0f };
				Math::decomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		}
	}
	void EditorLayer::UI_Stats()
	{
		int fps = (int)(1000.0f / m_Timestep);

		static Timer timer;
		static int index = 0;
		static float frametimeBank[100] = {0.0f};

		int averageFps = 0;
		float averageFrametime = 0.0f;
		static int avfps = 0;
		static float avft = 0.0f;
		static int lowestFps = 5000;
		static float highestFrametime = 0.0f;

		if (index >= 100) index = 0;

		frametimeBank[index] = m_Timestep;

		for (int i = 0; i < 100; i++)
			averageFrametime += frametimeBank[i];

		averageFrametime /= 100;
		averageFps = (int)(1000.0f / averageFrametime);
		index++;


		int timeElapsed = (int)timer.elapsedMillis();
		if (timeElapsed % 500 == 0 && timeElapsed > 0)
		{
			avfps = averageFps;
			avft = averageFrametime;
			timer.reset();
		}

		if (m_Timestep > highestFrametime)
		{
			highestFrametime = m_Timestep;
			lowestFps = fps;
		}

		ImGui::Begin("Statistics:", 0, ImGuiWindowFlags_NoNav);
		{
			ImGui::Text("Frametime: %.2fms", m_Timestep);
			ImGui::Text("Average Frametime: %.2fms", avft);
			ImGui::Text("Highest Frametime: %.2fms", highestFrametime);
			ImGui::Text("FPS: %d", fps);
			ImGui::Text("Average FPS: %d", avfps);
			ImGui::Text("Lowest FPS: %d", lowestFps);
			ImGui::Text("Profiling: %d", LG_PROFILE_IS_ACTIVE());
		}
		ImGui::Separator();
		
		{
			ImGui::Text("ImGui:");
			ImGui::Text("ImGui Active ID: %u", GImGui->ActiveId);
		}
		ImGui::Separator();

		ImGui::Text("Renderer2D:");
		ImGui::Text("Draw Calls: %d", Renderer2D::getStats().drawCalls);
		ImGui::Text("Quad Count: %d", Renderer2D::getStats().quadCount);
		ImGui::Text("Circle Count: %d", Renderer2D::getStats().circleCount);
		ImGui::Text("Vertex Count: %d", Renderer2D::getStats().getTotalVertexCount());
		ImGui::Text("Index Count: %d", Renderer2D::getStats().getTotalIndexCount());
		ImGui::Separator();
		ImGui::End();
	}
	void EditorLayer::UI_Settings()
	{
		ImGui::Begin("Editor Settings");
		ImGui::Checkbox("Show Colliders", &m_ShowColliders);
		ImGui::End();
	}
	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 2 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav);

		float size = ImGui::GetWindowHeight() - 4.0f;

		{
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			ref<SubTexture2D> icon = m_SceneState != SceneState::Play ? m_PlayIcon : m_StopIcon;
			if (ImGui::ImageButton((ImTextureID)icon->getID(), { size, size }, { 0,0 }, { 1, 1 }, 0))
			{
				if (m_SceneState != SceneState::Play)
					onScenePlay();
				else if (m_SceneState == SceneState::Play)
					onSceneStop();
			}
		}

		{
			ImGui::SameLine();
			ref<SubTexture2D> icon = m_SceneState != SceneState::Simulate ? m_SimulateIcon : m_StopIcon;
			if (ImGui::ImageButton((ImTextureID)icon->getID(), { size, size }, { 0,0 }, { 1, 1 }, 0))
			{
				if (m_SceneState != SceneState::Simulate)
					onSimutalionScenePlay();
				else if (m_SceneState == SceneState::Simulate)
					onSceneStop();
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
		ImGui::End();
	}
	void EditorLayer::UI_Header()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) Application::get().close();
					ImGui::EndMenu();
			}

			if (Project::getActive())
			{
				if (ImGui::BeginMenu("Scene"))
				{
					if (ImGui::MenuItem("New"))
						newScene();
					if (ImGui::MenuItem("Save"))
						saveScene();
					if (ImGui::MenuItem("Save as..."))
						saveSceneAs();

					ImGui::EndMenu();
				}
			}

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("New", "CTRL + N"))
					newProject();
				if (ImGui::MenuItem("Open...", "CTRL + O"))
					openProject();
				if (ImGui::MenuItem("Save", "CTRL + S"))
					saveProject();
				if (ImGui::MenuItem("Save as..."))
					saveProjectAs();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::onEvent(Event& e)
	{
		m_EditorCamera.onEvent(e);
		auto dispatcher = EventDispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::onKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
	}
	bool EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (m_SceneState != SceneState::Play)
		{
			bool control = Input::isKeyPressed(LgKey::LEFT_CONTROL) || Input::isKeyPressed(LgKey::RIGHT_CONTROL);
			bool alt = Input::isKeyPressed(LgKey::LEFT_ALT) || Input::isKeyPressed(LgKey::RIGHT_ALT);
			if (e.getMouseButton() == LG_MOUSE_BUTTON_LEFT && !control && !alt)
			{
				auto [mx, my] = ImGui::GetMousePos();
				mx -= m_ViewportBounds[0].x;
				my -= m_ViewportBounds[0].y;
				glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
				my = viewportSize.y - my;

				int mouseX = (int)mx;
				int mouseY = (int)my;

				if (mouseX >= 0 && mouseY >= 0 && mouseX <= (int)viewportSize.x && (int)mouseY <= viewportSize.y && !ImGuizmo::IsOver())
				{
					m_FrameBuffer->bind();
					int pixelData = m_FrameBuffer->readPixel(1, mouseX, mouseY);

					if (pixelData > -1)
						m_SceneHierarchyPanel.setSelectedEntity({ (entt::entity)pixelData, m_ActiveScene.get() });

					m_FrameBuffer->unbind();
				}
			}
		}

		return false;
	}
	bool EditorLayer::onKeyPressed(KeyPressedEvent& e)
	{
		if (e.getRepeatCount() > 0)
			return false;

		bool control = Input::isKeyPressed(LgKey::LEFT_CONTROL) || Input::isKeyPressed(LgKey::RIGHT_CONTROL);
		bool shift = Input::isKeyPressed(LgKey::LEFT_SHIFT) || Input::isKeyPressed(LgKey::RIGHT_SHIFT);

		switch (e.getKeyCode())
		{
			case LgKey::I:
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
			case LgKey::N:
			{
				if (control)
					newProject();
				break;
			}
			case LgKey::O:
			{
				if (control)
					openProject();
				break;
			}

			// Commands
			case LgKey::S:
			{
				if (shift)
					saveProjectAs();
				else if (control)
					saveProject();
				break;
			}
			case LgKey::D:
			{
				if (control)
					onDuplicateEntity();
				break;
			}
			case LgKey::DEL:
			{
				Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
				if (selectedEntity && !GImGui->ActiveId &&  m_ViewPortHovered)
				{
					m_SceneHierarchyPanel.setSelectedEntity({});
					m_ActiveScene->deleteEntity(selectedEntity);
				}
				break;
			}
			
			// Gizmos
			case LgKey::Q:
			{
				m_GizmoType = -1;
				break;
			}
			case LgKey::W:
			{
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case LgKey::E:
			{
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case LgKey::R:
			{
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
		default:
			break;
		}

		return false;
	}

	void EditorLayer::newProject()
	{
		std::string filepath = FileDialogs::saveFile("L3gion Project (*.lgproj)\0*.lgproj\0");
		if (!filepath.empty())
		{
			if (Project::getActive())
				saveProject();

			Project::newProject();
			Project::saveProject(filepath);
		}
	}
	bool EditorLayer::openProject()
	{
		std::string filepath = FileDialogs::openFile("L3gion Project (*.lgproj)\0*.lgproj\0");
		if (filepath.empty())
			return false;
		
		openProject(filepath);

		return true;
	}
	void EditorLayer::openProject(const std::filesystem::path& path)
	{
		if (Project::getActive())
			saveScene();
		Project::openProject(path);
		ScriptEngine::init();
	}
	void EditorLayer::saveProjectAs()
	{
		std::string filepath = FileDialogs::saveFile("L3gion Project (*.lgproj)\0*.lgproj\0");
		if (!filepath.empty())
			Project::saveProject(std::filesystem::path(filepath));

		//saveScene();
	}
	void EditorLayer::saveProject()
	{
		if (Project::getActive())
			Project::saveProject(Project::getRootDir());

		saveScene();
	}

	void EditorLayer::newScene()
	{
		onSceneStop();
		m_ActiveScene = createRef<Scene>();
		m_EditorScene = m_ActiveScene;
		m_SceneHierarchyPanel.setContext(m_ActiveScene);
		ScriptEngine::setContext(m_ActiveScene);
		m_EditorScenePath.clear();
	}
	void EditorLayer::openScene()
	{
		std::string filepath = FileDialogs::openFile("L3gion Scene (*.lg)\0*.lg\0");
		if (!filepath.empty())
			openScene(filepath);
	}
	void EditorLayer::openScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			onSceneStop();

		if (path.extension().string() != ".lg")
		{
			LG_CORE_WARN("Could not load {0}: Is not a scene file!", path.filename().string().c_str());
			return;
		}

		ref<Scene> newScene = createRef<Scene>();

		SceneSerializer serializer(newScene);
		if (serializer.deserialize(path.string()))
		{
			m_EditorScene = newScene;

			m_SceneHierarchyPanel.setContext(m_EditorScene);
			ScriptEngine::setContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}
	void EditorLayer::saveSceneAs()
	{
		std::string filepath = FileDialogs::saveFile("L3gion Scene (*.lg)\0*.lg\0");
		if (!filepath.empty())
		{
			m_EditorScenePath = filepath;
			saveScene();
		}
	}
	void EditorLayer::saveScene()
	{
		if (!m_EditorScenePath.empty())
		{
			serializeScene(m_EditorScene, m_EditorScenePath);
		}
		else
			saveSceneAs();

		if (m_ContentBrowserPanel)
			m_ContentBrowserPanel->refresh();
	}
	void EditorLayer::serializeScene(ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.serialize(path.string());
	}

	void EditorLayer::onScenePlay()
	{
		if (m_SceneState != SceneState::Edit)
			onSceneStop();

		if (m_EditorScene)
		{
			m_ActiveScene = Scene::copy(m_EditorScene);
			m_ActiveScene->onRuntimeStart();

			m_SceneHierarchyPanel.setContext(m_ActiveScene);
			ScriptEngine::setContext(m_ActiveScene);

			m_SceneState = SceneState::Play;
		}
	}
	void EditorLayer::onSceneStop()
	{
		//LG_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate, " ");

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->onRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->onSimulationStop();

		m_SceneState = SceneState::Edit;
		m_SceneHierarchyPanel.setContext(m_EditorScene);
		ScriptEngine::setContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;
	}
	void EditorLayer::onSimutalionScenePlay()
	{
		if (m_SceneState != SceneState::Edit)
			onSceneStop();

		if (m_EditorScene)
		{
			m_ActiveScene = Scene::copy(m_EditorScene);
			m_ActiveScene->onSimulationStart();

			m_SceneHierarchyPanel.setContext(m_ActiveScene);
			ScriptEngine::setContext(m_ActiveScene);

			m_SceneState = SceneState::Simulate;
		}
	}

	void EditorLayer::onDuplicateEntity()
	{
		if (m_SceneState == SceneState::Play)
			return;

		Entity selected = m_SceneHierarchyPanel.getSelectedEntity();
		if (selected)
			m_EditorScene->duplicateEntity(selected);
	}
}