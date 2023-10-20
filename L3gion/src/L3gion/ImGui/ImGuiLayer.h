#pragma once

#include "L3gion/Layer.h"

namespace L3gion
{
	class L3GION_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImGuiRender() override;

		void begin();
		void end();
	private:
		float m_Time = 0.0f;
	};
}

