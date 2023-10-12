#pragma once

#ifdef LG_PLATFORM_WINDOWS

extern L3gion::Application* L3gion::createApplication();

int main(int argc, char** argv)
{
	auto a = L3gion::createApplication();
	a->run();
	delete a;
}

#endif