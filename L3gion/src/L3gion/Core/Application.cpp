#include "lgpch.h"
#include "L3gion/Core/Application.h"

#include "L3gion/Events/MouseEvent.h"
#include "L3gion/Core/Input.h"

#include <glad/glad.h>
#include "L3gion/Renderer/Renderer.h"

#include "GLFW/glfw3.h"

namespace L3gion
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{	
		LG_PROFILE_FUNCTION();

		LG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::create(WindowProps(name));
		m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));

		Renderer::init();

		m_ImGuiLayer = new ImGuiLayer;
		pushOverlay(m_ImGuiLayer);

		m_Running = true;
	}

	Application::~Application()
	{
		LG_PROFILE_FUNCTION();

		Renderer::shutdown();
	}

	void Application::pushLayer(Layer* layer)
	{
		LG_PROFILE_FUNCTION();

		m_LayerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* overlay)
	{
		LG_PROFILE_FUNCTION();

		m_LayerStack.pushOverlay(overlay);
		overlay->onAttach();
	}

	void Application::onEvent(Event& e)
	{
		LG_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->onEvent(e);
			if (e.isHandled())
				break;
		}
	}

	void Application::run()
	{
		LG_PROFILE_FUNCTION();

		while (m_Running)
		{
			LG_PROFILE_SCOPE("Run Loop");
			double time = glfwGetTime(); // Should be outside this class
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					LG_PROFILE_SCOPE("LayerStack: OnUpdates");

					for (Layer* layer : m_LayerStack)
						layer->onUpdate(timestep);
				}

				{
					LG_PROFILE_SCOPE("LayerStack: OnIMGUIRendering");

					// ImGui rendering
					m_ImGuiLayer->begin();
					for (Layer* layer : m_LayerStack)
						layer->onImGuiRender();
					m_ImGuiLayer->end();
				}
			}
			m_Window->onUpdate();
		}
	}

	bool Application::onWindowResize(WindowResizeEvent& e)
	{
		LG_PROFILE_FUNCTION();

		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::onWindowResize(e.getWidth(), e.getHeight());

		return false;
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;

		return true;
	}
}