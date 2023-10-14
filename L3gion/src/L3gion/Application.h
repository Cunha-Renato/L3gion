#pragma once

#include "Core.h"

#include "Window.h"
#include "L3gion/LayerStack.h"
#include "Events/Event.h"
#include "L3gion/Events/ApplicationEvent.h"

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

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		inline Window& getWindow() { return *m_Window; }
		inline static Application& get() { return *s_Instance; }

	private:
		bool onWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running;
		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* createApplication();
}