#pragma once

#include "L3gion/Core/Core.h"
#include "L3gion/Core/Application.h"

#ifdef LG_PLATFORM_WINDOWS

extern L3gion::Application* L3gion::createApplication(L3gion::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	LG_PROFILE_BEGIN_SESSION("Startup", "L3gionProfile_Startup.json");
	L3gion::Log::init();
	auto app = L3gion::createApplication({argc, argv});
	LG_PROFILE_END_SESSION();
	
	app->run();
	
	LG_PROFILE_BEGIN_SESSION("Shutdown", "L3gionProfile_Sutdown.json");
	delete app;
	LG_PROFILE_END_SESSION();
}

#endif