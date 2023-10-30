#pragma once

#include "L3gion/Core/Layer.h"

namespace L3gion
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onEvent(Event& e) override;

		void begin();
		void end();

		void blockEvents(bool block) { m_BlockEvents = block; }
	
		void setDarkTheme();
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}

