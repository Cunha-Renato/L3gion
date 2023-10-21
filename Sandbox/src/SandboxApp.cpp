#include <L3gion.h>
#include "ImGui/imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public L3gion::Layer
{
public:
	ExampleLayer(std::string name)
		: Layer(name), m_OrthoCamera(-1.0f, 1.0f, -1.0f, 1.0f), m_CameraPosition(0.0f), m_Color(0.0f)
	{
		// OpenGL buffers
		m_VertexArray = L3gion::VertexArray::create();

		float vertices[] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		L3gion::ref<L3gion::VertexBuffer> vertexBuffer;
		vertexBuffer = L3gion::VertexBuffer::create(vertices, sizeof(vertices));

		L3gion::BufferLayout layout =
		{
			{L3gion::ShaderDataType::Float3, "a_Position"},
			{L3gion::ShaderDataType::Float2, "a_TextCoord"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		L3gion::ref<L3gion::IndexBuffer> indexBuffer;
		indexBuffer = L3gion::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->setIndexBuffer(indexBuffer);

		// Shaders
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TextCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TextCoord;

			void main()
			{
				v_TextCoord = a_TextCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 v_TextCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TextCoord);
			}
		)";

		m_Shader = L3gion::Shader::create(vertexSrc, fragmentSrc);

		m_Texture = L3gion::Texture2D::create("assets/textures/checkerboard.png");

		std::dynamic_pointer_cast<L3gion::OpenGLShader>(m_Shader)->bind();
		std::dynamic_pointer_cast<L3gion::OpenGLShader>(m_Shader)->setInt("u_Texture", 0);
	}

	virtual void onUpdate(L3gion::Timestep& ts) override
	{
		L3gion::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		L3gion::RenderCommand::clear();
		
		if (L3gion::Input::isKeyPressed(LG_KEY_LEFT))
			m_CameraPosition.x += m_CameraSpeed * (float)ts;
		
		if (L3gion::Input::isKeyPressed(LG_KEY_RIGHT))
			m_CameraPosition.x -= m_CameraSpeed * (float)ts;
		
		if (L3gion::Input::isKeyPressed(LG_KEY_UP))
			m_CameraPosition.y -= m_CameraSpeed * (float)ts;
		
		if (L3gion::Input::isKeyPressed(LG_KEY_DOWN))
			m_CameraPosition.y += m_CameraSpeed * (float)ts;

		m_OrthoCamera.setPosition(m_CameraPosition);

		L3gion::Renderer::beginScene(m_OrthoCamera);

		m_Texture->bind();
		L3gion::Renderer::submit(m_Shader, m_VertexArray);

		L3gion::Renderer::endScene();
	}

	virtual void onImGuiRender() override
	{
		//ImGui::Begin("Settings");
		//
		//ImGui::ColorEdit3("Color", glm::value_ptr(m_Color));
		//
		//ImGui::End();
	}

private:
	L3gion::ref<L3gion::Shader> m_Shader;
	L3gion::ref<L3gion::VertexArray> m_VertexArray;

	L3gion::ref<L3gion::Texture2D> m_Texture;

	glm::vec4 m_Color;

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
		setVsync(false);
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