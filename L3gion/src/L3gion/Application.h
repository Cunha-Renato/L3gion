#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "L3gion/Events/ApplicationEvent.h"
#include "L3gion/Events/KeyEvent.h"
#include "L3gion/Events/MouseEvent.h"

namespace L3gion
{
	class L3GION_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
		
		void onEvent(Event& e);

		inline void setVsync(bool option) { m_Window->setVSync(option); }
	private:
		bool onWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	// To be defined in CLIENT
	Application* createApplication();
}