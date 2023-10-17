#include "lgpch.h"
#include "Application.h"

#include "L3gion/Events/MouseEvent.h"
#include "Input.h"

#include <glad/glad.h>

namespace L3gion
{
	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case L3gion::ShaderDataType::Float:		return GL_FLOAT;
			case L3gion::ShaderDataType::Float2:	return GL_FLOAT;
			case L3gion::ShaderDataType::Float3:	return GL_FLOAT;
			case L3gion::ShaderDataType::Float4:	return GL_FLOAT;
			case L3gion::ShaderDataType::Mat3:		return GL_FLOAT;
			case L3gion::ShaderDataType::Mat4:		return GL_FLOAT;
			case L3gion::ShaderDataType::Int:		return GL_INT;
			case L3gion::ShaderDataType::Int2:		return GL_INT;
			case L3gion::ShaderDataType::Int3:		return GL_INT;
			case L3gion::ShaderDataType::Int4:		return GL_INT;
			case L3gion::ShaderDataType::Bool:		return GL_BOOL;
		}

		LG_CORE_ASSERT(false, "In ShaderDataTypeToOpenGLBaseType(): Unknown ShaderDataType!");
			
		return 0;
	}

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

		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		};

		m_VertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		BufferLayout layout = 
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};

		m_VertexBuffer->setLayout(layout);

		uint32_t index = 0;
		for (const auto& element : m_VertexBuffer->getLayout())
		{
			// Structure of the data
			glEnableVertexAttribArray(index);
			
			glVertexAttribPointer(index, 
				element.getComponentCount(), 
				ShaderDataTypeToOpenGLBaseType(element.type), 
				element.normalized ? GL_TRUE : GL_FALSE, 
				layout.getStride(), 
				(const void*)element.offset
			);

			index++;
		}

		uint32_t indices[] = {0, 1, 2};

		m_IndexBuffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));

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

			m_Shader->bind();

			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);

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