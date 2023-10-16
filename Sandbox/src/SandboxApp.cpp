#include <L3gion.h>
#include "ImGui/imgui.h"


class ExampleLayer : public L3gion::Layer
{
	public:
		ExampleLayer(std::string name)
			: Layer(name){}
		virtual void onImGuiRender() override
		{
			static bool show = true;
			ImGui::ShowDemoWindow(&show);
		}
};

class Sandbox : public L3gion::Application
{
private: 
		ExampleLayer* m_Layer; 
public:
	Sandbox()
	{
		m_Layer = new ExampleLayer("Example Layer");
		pushLayer(m_Layer);
	}

	~Sandbox()
	{
		delete m_Layer;
	}
};

L3gion::Application* L3gion::createApplication()
{
	Application* app = new Sandbox();
	return app;
}