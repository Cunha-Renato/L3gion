#pragma once

#ifdef LG_PLATFORM_WINDOWS

extern L3gion::scope<L3gion::Application> L3gion::createApplication();

int main(int argc, char** argv)
{
	L3gion::Log::init();
	auto app = L3gion::createApplication();
	app->run();
}

#endif