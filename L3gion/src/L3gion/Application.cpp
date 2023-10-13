#include "Application.h"
#include "L3gion/Events/MouseEvent.h"
#include "Log.h"

#include <iostream>

namespace L3gion
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		L3gion::Log::init();
		
		MouseButtonPressedEvent e(12);
		LG_WARN(e);
		for (;;);
	}
}