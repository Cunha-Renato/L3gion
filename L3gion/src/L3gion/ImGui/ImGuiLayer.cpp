#include "lgpch.h"
#include "ImGuiLayer.h"

#include "L3gion/Application.h"
#include "L3gion/KeyCodes.h"
#include "imgui.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"
#include "Platform/OpenGL/imgui_impl_glfw.h"

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

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui::StyleColorsDark();

		auto nativeWindow = Application::get().getWindow().getNativeWindow();
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(nativeWindow), true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::onDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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
		ImGui_ImplGlfw_NewFrame();
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

		return false;
	}

	bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(e.getMouseButton(), false);

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

		return false;
	}

	bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiKey key = static_cast<ImGuiKey>(LgKeyToImGui(e.getKeyCode()));
		io.AddKeyEvent(key, true);
		
		// Error Here
		io.KeyCtrl = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
		io.KeyShift = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);
		io.KeyAlt = ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt);
		io.KeySuper = ImGui::IsKeyDown(ImGuiKey_LeftSuper) || ImGui::IsKeyDown(ImGuiKey_RightSuper);

		return false;
	}

	bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiKey key = static_cast<ImGuiKey>(LgKeyToImGui(e.getKeyCode()));
		io.AddKeyEvent(key, false);

		// Error Here
		io.KeyCtrl = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
		io.KeyShift = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);
		io.KeyAlt = ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt);
		io.KeySuper = ImGui::IsKeyDown(ImGuiKey_LeftSuper) || ImGui::IsKeyDown(ImGuiKey_RightSuper);

		return false;
	}

	// For typing the codes are universal
	bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e)
	{
		//ImGuiIO& io = ImGui::GetIO();
		//int keyCode = e.getKeyCode();
		//if( keyCode > 0 && keyCode < 0x10000)
		//	io.AddInputCharacter((unsigned short)keyCode);

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