#pragma once

#include "Core.h"

#include "Window.h"
#include "L3gion/LayerStack.h"
#include "Events/Event.h"
#include "L3gion/Events/ApplicationEvent.h"

#include "L3gion/ImGui/ImGuiLayer.h"

#include "L3gion/Renderer/Shader.h"
#include "L3gion/Renderer/Buffer.h"
#include "L3gion/Renderer/VertexArray.h"

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
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* createApplication();
}