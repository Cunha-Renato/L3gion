#include "lgpch.h"
#include "Application.h"

#include "L3gion/Events/MouseEvent.h"
#include "Input.h"

#include <glad/glad.h>
#include "L3gion/Renderer/Renderer.h"

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

		m_VertexArray.reset(VertexArray::create());

		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		BufferLayout layout =
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = {0, 1, 2};

		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->setIndexBuffer(indexBuffer);

		// Shaders
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		m_Running = true;
	}

	Application::~Application()
	{
		delete m_ImGuiLayer;
	}

	void Application::pushLayer(Layer* layer)
	{
		m_LayerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* overlay)
	{
		m_LayerStack.pushOverlay(overlay);
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
			RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
			RenderCommand::clear();

			Renderer::beginScene();

			m_Shader->bind();
			Renderer::submit(m_VertexArray);

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