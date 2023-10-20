#include "lgpch.h"
#include "KeyCodes.h"

namespace L3gion
{
	int GlfwKeyToLG(int keycode)
	{
		auto it = std::find(glfwKeysBuffer.begin(), glfwKeysBuffer.end(), keycode);

		if (it != glfwKeysBuffer.end())
			return (int)(std::distance(glfwKeysBuffer.begin(), it) + 1);

		LG_CORE_ERROR("The GLFW_KEY provided ({0}) does not match the LG_KEYs!!", keycode);

		return 0;
	}

	int ImGuiKeyToLG(int keycode)
	{
		auto it = std::find(imGuiKeysBuffer.begin(), imGuiKeysBuffer.end(), keycode);

		if (it != imGuiKeysBuffer.end())
			return (int)(std::distance(imGuiKeysBuffer.begin(), it) + 1);

		LG_CORE_ERROR("The IMGUI_KEY provided ({0}) does not match the LG_KEYs!!", keycode);

		return 0;
	}
}