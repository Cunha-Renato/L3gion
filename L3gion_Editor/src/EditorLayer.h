#pragma once

#include "L3gion.h"
#include "L3gion/Renderer/Texture.h"

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
		OrthoCameraController m_CameraController;

		ref<FrameBuffer> m_FrameBuffer;

		ref<Texture2D> m_SpriteSheetTexture;
		ref<SubTexture2D> m_Checkerboard;
		ref<SubTexture2D> m_Tree;

		glm::vec4 m_Color = { 0.8f, 0.2f, 0.3f, 1.0f };

		bool m_ViewPortFocused = false, m_ViewPortHovered = false;

		glm::vec2 m_ViewPortSize = { 0, 0 };
		double m_Timesep = 0;
	};

}