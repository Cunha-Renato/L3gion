#include "lgpch.h"
#include "L3gion/ImGui/ImGuiLayer.h"
#include "L3gion/Core/Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "ImGuizmo.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace L3gion
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::onAttach()
	{
		LG_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		IMGUI_HAS_DOCK true;
		IMGUI_HAS_VIEWPORT true;
		
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigDockingTransparentPayload = true;
		//io.ConfigFlags |= ImGuiViewportFlags_NoTaskBarIcon;
		//io.ConfigFlags |= ImGuiViewportFlags_NoAutoMerge;

		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Bold.ttf", 17.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Regular.ttf", 17.0f);

		// Setup Imgui style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		setDarkTheme();

		Application& app = Application::get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}
	void ImGuiLayer::onDetach()
	{
		LG_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiLayer::onEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.handled |= e.isInCategory(EC_Mouse) & io.WantCaptureMouse;
			e.handled |= e.isInCategory(EC_Keyboard) & io.WantCaptureKeyboard;
		}
	}
	void ImGuiLayer::begin()
	{
		LG_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}
	void ImGuiLayer::end()
	{
		LG_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2((float)app.getWindow().getWidth(), (float)app.getWindow().getHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// View port stuff
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::setDarkTheme()
	{
		ImVec4 orange = { 0.8f, 0.4, 0.21f, 1.0f };
		ImVec4 fadedOrange = { 0.8f, 0.5, 0.31f, 1.0f };
		ImVec4 dark = { 0.09f, 0.07f, 0.07f, 1.0f };
		ImVec4 light = { 0.2f, 0.18f, 0.18f, 1.0f };

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = dark;
		colors[ImGuiCol_ChildBg] = dark;
		colors[ImGuiCol_MenuBarBg] = dark;

		// Headers
		colors[ImGuiCol_Header] = light;
		colors[ImGuiCol_HeaderHovered] = orange;
		colors[ImGuiCol_HeaderActive] = fadedOrange;

		// Buttons
		colors[ImGuiCol_Button] = light;
		colors[ImGuiCol_ButtonHovered] = orange;
		colors[ImGuiCol_ButtonActive] = fadedOrange;

		// Frame BG
		colors[ImGuiCol_FrameBg] = light;
		colors[ImGuiCol_FrameBgHovered] = light;
		colors[ImGuiCol_FrameBgActive] = orange;

		// Tabs
		colors[ImGuiCol_Tab] = dark;
		colors[ImGuiCol_TabHovered] = fadedOrange;
		colors[ImGuiCol_TabActive] = orange;
		colors[ImGuiCol_TabUnfocused] = dark;
		colors[ImGuiCol_TabUnfocusedActive] = dark;

		// Title
		colors[ImGuiCol_TitleBg] = dark;
		colors[ImGuiCol_TitleBgActive] = dark;
		colors[ImGuiCol_TitleBgCollapsed] = dark;

		// Resize
		colors[ImGuiCol_ResizeGrip] = orange;
		colors[ImGuiCol_ResizeGripHovered] = fadedOrange;
		colors[ImGuiCol_ResizeGripActive] = orange;

		// Navigation
		colors[ImGuiCol_NavWindowingHighlight] = orange;
		colors[ImGuiCol_ScrollbarGrabActive] = orange;
		colors[ImGuiCol_NavHighlight] = orange;

		// Tools
		colors[ImGuiCol_PopupBg] = dark;
		colors[ImGuiCol_DockingPreview] = light;
		colors[ImGuiCol_DockingEmptyBg] = dark;
		colors[ImGuiCol_CheckMark] = orange;
	}
}