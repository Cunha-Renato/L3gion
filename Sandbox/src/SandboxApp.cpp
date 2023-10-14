#include <L3gion.h>

class Sandbox : public L3gion::Application
{
public:
	Sandbox()
	{
		pushOverlay(new L3gion::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

L3gion::Application* L3gion::createApplication()
{
	Application* app = new Sandbox();
	app->setVsync(false);
	return app;
}