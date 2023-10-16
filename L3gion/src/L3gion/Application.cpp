#include "lgpch.h"
#include "Application.h"

#include "L3gion/Events/MouseEvent.h"
#include "Input.h"

#include <glad/glad.h>

namespace L3gion
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		LG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::create());
		m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));

		m_ImGuiLayer = new ImGuiLayer;
		pushOverlay(m_ImGuiLayer);

		// OpenGL buffers
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};

		// Send data to OpenGL
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Structure of the data
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		unsigned int indices[] = {0, 1, 2};

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		m_Running = true;
	}

	Application::~Application()
	{
		delete m_ImGuiLayer;
	}

	void Application::pushLayer(Layer* layer)
	{
		m_LayerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* overlay)
	{
		m_LayerStack.pushOverlay(overlay);
		overlay->onAttach();
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->onEvent(e);
			if (e.isHandled())
				break;
		}
	}

	void Application::run()
	{
		while (m_Running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)
				layer->onUpdate();

			// ImGui rendering
			m_ImGuiLayer->begin();
			for (Layer* layer : m_LayerStack)
				layer->onImGuiRender();
			m_ImGuiLayer->end();

			m_Window->onUpdate();
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;

		return true;
	}
}