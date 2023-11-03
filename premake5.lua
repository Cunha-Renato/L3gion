include "./vendor/premake/premake_custom/solution_items.lua"
include "Dependencies.lua"
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