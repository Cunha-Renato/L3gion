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
	include "L3gion/vendor/box2d"

group ""

group "Core"
	include "L3gion"
	include "L3gion_ScriptCore"
group ""

group "Tools"
	include "L3gion_Editor"
group ""

group "Misc"
	include "Sandbox"
group ""