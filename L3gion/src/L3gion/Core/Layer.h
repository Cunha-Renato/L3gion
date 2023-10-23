#pragma once

#include "L3gion/Core/Core.h"
#include "L3gion/Events/Event.h"

#include "L3gion/Core/Timestep.h"

namespace L3gion
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep& timestep) {}
		virtual void onImGuiRender() {}
		virtual void onEvent(Event& event) {}

		inline const std::string& getName() const { return m_DebugName; }
	
	private:
		std::string m_DebugName;
	};
}
