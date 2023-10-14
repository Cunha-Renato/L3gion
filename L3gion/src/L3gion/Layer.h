#pragma once

#include "L3gion/Core.h"
#include "L3gion/Events/Event.h"

namespace L3gion
{
	class L3GION_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate() {}
		virtual void onEvent(Event& event) {}

		inline const std::string& getName() const { return m_DebugName; }
	
	private:
		std::string m_DebugName;
	};
}
