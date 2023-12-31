
-- L3gion Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/L3gion/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/L3gion/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/L3gion/vendor/imgui"
IncludeDir["filewatch"] = "%{wks.location}/L3gion/vendor/filewatch"
IncludeDir["glm"] = "%{wks.location}/L3gion/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/L3gion/vendor/stb_image"
IncludeDir["EnTT"] = "%{wks.location}/L3gion/vendor/EnTT/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/L3gion/vendor/yaml_cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/L3gion/vendor/ImGuizmo"
IncludeDir["VulkanSDK"] = "%{wks.location}/L3gion/vendor/VulkanSDK/include"
IncludeDir["box2d"] = "%{wks.location}/L3gion/vendor/box2d/include"
IncludeDir["mono"] = "%{wks.location}/L3gion/vendor/mono/include"

LibraryDir = {}

LibraryDir["mono"] = "%{wks.location}/L3gion/vendor/mono/lib/%{cfg.buildcfg}"
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/L3gion/vendor/VulkanSDK/Lib"

Library = {}

Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["Winmm"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["Bcrypt"] = "Bcrypt.lib"