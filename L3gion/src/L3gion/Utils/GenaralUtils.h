#pragma once

#include "GLFW/glfw3.h"

namespace L3gion::Utils
{
	class Time
	{
	public:
		static double getTime() { return glfwGetTime(); }
	};
}