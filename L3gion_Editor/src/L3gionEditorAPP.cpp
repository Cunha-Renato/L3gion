#include <L3gion.h>
#include <L3gion/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace L3gion
{
	class L3gionEditorApp : public Application
	{
	public:
		L3gionEditorApp(L3gion::ApplicationCommandLineArgs args)
			: Application("L3gion Editor", args)
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
		return new L3gionEditorApp(args);
	}
}