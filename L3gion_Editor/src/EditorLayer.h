#pragma once

#include "L3gion.h"
#include "L3gion/Renderer/Texture.h"
#include "L3gion/Scene/Scene.h"
#include "L3gion/Scene/Entity.h"
#include "L3gion/Renderer/EditorCamera.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace L3gion
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer(const std::string& name);
		~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onUpdate(Timestep& ts) override;
		virtual void onImGuiRender() override;

		virtual void onEvent(Event& e) override;
	
	private:
		bool onKeyPressed(KeyPressedEvent& e);
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
			
		void newScene();
		void openScene();
		void openScene(const std::string& path);
		void saveSceneAs();

		void onScenePlay();
		void onSceneStop();

		//UI Functions
		void UI_Toolbar();
		void UI_Settings();
		void UI_Gizmos();
		void UI_Header();

		template<typename Func>
		void UI_Viewport(Func func);

	private:
		ref<Framebuffer> m_FrameBuffer;

		ref<Scene> m_ActiveScene = nullptr;
		EditorCamera m_EditorCamera;

		bool m_ViewPortFocused = false, m_ViewPortHovered = false;
		glm::vec2 m_ViewPortSize = { 0, 0 };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		enum class SceneState
		{
			Edit = 0,
			Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;
		ref <SubTexture2D> m_PlayIcon, m_StopIcon;
		
		double m_Timesep = 0;
	};

}