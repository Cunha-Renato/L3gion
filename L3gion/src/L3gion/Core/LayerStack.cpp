#include "lgpch.h"
#include "L3gion/Core/LayerStack.h"

namespace L3gion
{
	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->onDetach();
			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::pushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::popLayer(Layer* layer)
	{
		// Finds the layer in the m_Layers
		auto iterator = std::find(m_Layers.begin(), m_Layers.end(), layer);
		
		if (iterator != m_Layers.end())
		{
			layer->onDetach();
			// Erases it from the list
			m_Layers.erase(iterator);

			// Subtract one from te insert iterator
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::popOverlay(Layer* ovelay)
	{
		// Finds the layer in the m_Layers
		auto iterator = std::find(m_Layers.begin(), m_Layers.end(), ovelay);

		if (iterator != m_Layers.end())
		{
			ovelay->onDetach();
			// Erases it from the list
			m_Layers.erase(iterator);
		}
		
	}
}