#include <L3gion.h>

class Sandbox : public L3gion::Application
{
public:
	Sandbox()
	{
	}

	~Sandbox()
	{

	}
};

L3gion::Application* L3gion::createApplication()
{
	return new Sandbox();
}