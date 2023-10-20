#include <L3gion.h>
#include "ImGui/imgui.h"


class ExampleLayer : public L3gion::Layer
{
public:
	ExampleLayer(std::string name)
		: Layer(name), m_OrthoCamera(-1.0f, 1.0f, -1.0f, 1.0f), m_CameraPosition(0.0f)
	{
		// OpenGL buffers
		m_VertexArray.reset(L3gion::VertexArray::create());

		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		};

		std::shared_ptr<L3gion::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(L3gion::VertexBuffer::create(vertices, sizeof(vertices)));

		L3gion::BufferLayout layout =
		{
			{L3gion::ShaderDataType::Float3, "a_Position"},
			{L3gion::ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2 };

		std::shared_ptr<L3gion::IndexBuffer> indexBuffer;
		indexBuffer.reset(L3gion::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->setIndexBuffer(indexBuffer);

		// Shaders
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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

		m_Shader.reset(new L3gion::Shader(vertexSrc, fragmentSrc));
	}
	virtual void onImGuiRender() override
	{
			
	}

	virtual void onUpdate(L3gion::Timestep& ts) override
	{
		LG_TRACE("Delta time: {0}s, ({1}ms)", ts.getSeconds(), ts.getMiliSeconds());
		
		if (L3gion::Input::isKeyPressed(LG_KEY_LEFT))
			m_CameraPosition.x += m_CameraSpeed * ts;
		
		if (L3gion::Input::isKeyPressed(LG_KEY_RIGHT))
			m_CameraPosition.x -= m_CameraSpeed * ts;
		
		if (L3gion::Input::isKeyPressed(LG_KEY_UP))
			m_CameraPosition.y -= m_CameraSpeed * ts;
		
		if (L3gion::Input::isKeyPressed(LG_KEY_DOWN))
			m_CameraPosition.y += m_CameraSpeed * ts;

		L3gion::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		L3gion::RenderCommand::clear();

		m_OrthoCamera.setPosition(m_CameraPosition);

		L3gion::Renderer::beginScene(m_OrthoCamera);

		m_Shader->bind();
		L3gion::Renderer::submit(m_Shader, m_VertexArray);
	}

private:
	std::shared_ptr<L3gion::Shader> m_Shader;
	std::shared_ptr<L3gion::VertexArray> m_VertexArray;

	L3gion::OrthoCamera m_OrthoCamera;
	glm::vec3 m_CameraPosition;
	float m_CameraSpeed = 1.0f;
};

//------------------------- SANDBOX_APPLICATION ---------------------

class Sandbox : public L3gion::Application
{
private: 
		ExampleLayer* m_Layer; 
public:
	Sandbox()
	{
		m_Layer = new ExampleLayer("Example Layer");
		pushLayer(m_Layer);
		setVsync(true);
	}

	~Sandbox()
	{
	}
};

L3gion::Application* L3gion::createApplication()
{
	Application* app = new Sandbox();
	return app;
}