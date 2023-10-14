#include "lgpch.h"
#include "ImGuiLayer.h"

#include "L3gion/Application.h"
#include "imgui.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace L3gion
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{

	}
	void ImGuiLayer::onAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::onDetach()
	{

	}
	void ImGuiLayer::onUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::onEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ImGuiLayer::onMouseButtonPresedEvent));
		dispatcher.dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(ImGuiLayer::onMouseButtonReleasedEvent));
		dispatcher.dispatch<MouseMovedEvent>(BIND_EVENT_FN(ImGuiLayer::onMouseMoveEvent));
		dispatcher.dispatch<MouseScrollEvent>(BIND_EVENT_FN(ImGuiLayer::onMouseScrollEvent));

		dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FN(ImGuiLayer::onKeyPressedEvent));
		dispatcher.dispatch<KeyReleasedEvent>(BIND_EVENT_FN(ImGuiLayer::onKeyReleasedEvent));
		dispatcher.dispatch<KeyTypedEvent>(BIND_EVENT_FN(ImGuiLayer::onKeyTypedEvent));

		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(ImGuiLayer::onWindowResizeEvent));
	}

	bool ImGuiLayer::onMouseButtonPresedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(e.getMouseButton(), true);

		LG_CORE_TRACE(e);

		return false;
	}

	bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(e.getMouseButton(), false);

		LG_CORE_TRACE(e);

		return false;
	}

	bool ImGuiLayer::onMouseMoveEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMousePosEvent(e.getX(), e.getY());

		return false;
	}

	bool ImGuiLayer::onMouseScrollEvent(MouseScrollEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(e.getXOffset(), e.getYOffset());

		LG_CORE_TRACE(e);

		return false;
	}

	bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		LG_CORE_TRACE(e);

		return false;
	}

	bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = false;

		LG_CORE_TRACE(e);

		return false;
	}

	bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keyCode = e.getKeyCode();
		if( keyCode > 0 && keyCode < 0x10000)
			io.AddInputCharacter((unsigned short)keyCode);

		LG_CORE_TRACE(e);

		return false;
	}

	bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.getWidth(), e.getHeight());
		
		return false;
	}
}