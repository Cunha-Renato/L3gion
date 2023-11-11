project "L3gion_ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/L3gion_Editor/Resources/scripts")
	objdir ("%{wks.location}/L3gion_Editor/Resources/scripts/Intermediates")

	files
	{
		"Source/**cs",
		"Properties/**cs",
	}

	filter "configurations:Debug"	
		optimize "Off"
		symbols "Default"
	
	filter "configurations:Release"	
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"	
		optimize "Full"
		symbols "Off"

			
