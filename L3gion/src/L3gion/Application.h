#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace L3gion
{
	class L3GION_API Application
	{
	public:
	public:
		Application();
		virtual ~Application();

		void run();
	private:
		
	
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	// To be defined in CLIENT
	Application* createApplication();
}