#pragma once

#include "L3gion/Layer.h"
#include "L3gion/Events/ApplicationEvent.h"
#include "L3gion/Events/KeyEvent.h"
#include "L3gion/Events/MouseEvent.h"


namespace L3gion
{
	class L3GION_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onAttach();
		void onDetach();
		void onUpdate();
		void onEvent(Event& event);

	private:
		bool onMouseButtonPresedEvent(MouseButtonPressedEvent& e);
		bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool onMouseMoveEvent(MouseMovedEvent& e);
		bool onMouseScrollEvent(MouseScrollEvent& e);
		bool onKeyPressedEvent(KeyPressedEvent& e);
		bool onKeyReleasedEvent(KeyReleasedEvent& e);
		bool onKeyTypedEvent(KeyTypedEvent& e);
		bool onWindowResizeEvent(WindowResizeEvent& e);

	private:
		float m_Time = 0;
	};
}

