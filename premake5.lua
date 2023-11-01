workspace "L3gion"
	architecture "x86_64"
	startproject "L3gion_Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/L3gion/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/L3gion/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/L3gion/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/L3gion/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/L3gion/vendor/stb_image"
IncludeDir["EnTT"] = "%{wks.location}/L3gion/vendor/EnTT/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/L3gion/vendor/yaml_cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/L3gion/vendor/ImGuizmo"

group "Dependencies"
	include "vendor/premake"
	include "L3gion/vendor/GLFW"
	include "L3gion/vendor/Glad"
	include "L3gion/vendor/imgui"
	include "L3gion/vendor/yaml_cpp"

group ""

include "L3gion"
include "L3gion_Editor"
include "Sandbox"