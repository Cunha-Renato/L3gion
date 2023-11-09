#include <L3gion.h>
#include <L3gion/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace L3gion
{
	class L3gionEditorApp : public Application
	{
	public:
		L3gionEditorApp(const ApplicationSpecs& specs)
			: Application(specs)
		{
			pushLayer(new EditorLayer("Editor Layer"));
			setVsync(false);
		}

		~L3gionEditorApp()
		{
		}
	};

	Application* createApplication(L3gion::ApplicationCommandLineArgs args)
	{
		ApplicationSpecs specs;
		specs.name = "L3gion Editor";
		specs.commandArgs = args;

		return new L3gionEditorApp(specs);
	}
}