#include "lgpch.h"
#include "LayerStack.h"

namespace L3gion
{
	LayerStack::LayerStack()
	{
		m_LayerInsert = m_Layers.begin();
	}

	LayerStack::~LayerStack()
	{
		for(Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::pushLayer(Layer* layer)
	{
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);
		m_LayerInsert++;
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
			// Erases it from the list
			m_Layers.erase(iterator);

			// Subtract one from te insert iterator
			m_LayerInsert--;
		}
	}

	void LayerStack::popOverlay(Layer* ovelay)
	{
		// Finds the layer in the m_Layers
		auto iterator = std::find(m_Layers.begin(), m_Layers.end(), ovelay);

		if (iterator != m_Layers.end())
			// Erases it from the list
			m_Layers.erase(iterator);
		
	}
}