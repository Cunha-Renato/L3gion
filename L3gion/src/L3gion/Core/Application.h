#pragma once

#include "L3gion/Core/Core.h"

#include "Window.h"
#include "L3gion/Core/LayerStack.h"
#include "L3gion/Events/Event.h"
#include "L3gion/Events/ApplicationEvent.h"

#include "L3gion/ImGui/ImGuiLayer.h"

#include "L3gion/Renderer/Shader.h"
#include "L3gion/Renderer/Buffer.h"
#include "L3gion/Renderer/VertexArray.h"
#include "L3gion/Renderer/Camera.h"

#include "L3gion/Core/Timestep.h"

namespace L3gion
{
	class Application
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
		bool onWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = false;
		bool m_Minimized = false;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		double m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	scope<Application> createApplication();
}