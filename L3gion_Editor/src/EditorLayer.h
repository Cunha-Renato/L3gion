#pragma once

#include "L3gion.h"
#include "L3gion/Renderer/Texture.h"
#include "L3gion/Scene/Scene.h"
#include "L3gion/Scene/Entity.h"
#include "Panels/SceneHierarchyPanel.h"

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
		bool onKeyPressed(Event& e);

	private:
		ref<FrameBuffer> m_FrameBuffer;

		ref<Scene> m_ActiveScene;
		Entity m_Square;
		Entity m_CameraEntity;

		SceneHierarchyPanel m_SceneHierarchyPanel;

		bool m_ViewPortFocused = false, m_ViewPortHovered = false;
		glm::vec2 m_ViewPortSize = { 0, 0 };

		double m_Timesep = 0;
	};

}