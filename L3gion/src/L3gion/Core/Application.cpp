#include "lgpch.h"
#include "L3gion/Core/Application.h"

#include "L3gion/Utils/GenaralUtils.h"
#include "L3gion/Renderer/Renderer.h"
#include "L3gion/Scripting/ScriptEngine.h"
#include "L3gion/Events/MouseEvent.h"
#include "L3gion/Core/Input.h"

namespace L3gion
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecs& specs)
		: m_Specs(specs)
	{	
		LG_PROFILE_FUNCTION();

		LG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// Setting working directory
		if (!m_Specs.workingDir.empty())
			std::filesystem::current_path(m_Specs.workingDir);

		m_Window = Window::create(WindowProps(m_Specs.name));
		m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));

		Renderer::init();
		ScriptEngine::init();

		m_ImGuiLayer = new ImGuiLayer;
		pushOverlay(m_ImGuiLayer);

		m_Running = true;
	}

	Application::~Application()
	{
		LG_PROFILE_FUNCTION();
		
		ScriptEngine::shutdown();
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
			
			double time = Utils::Time::getTime(); // Should be outside this class
			
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