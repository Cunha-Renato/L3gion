#include <L3gion.h>
#include <L3gion/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace L3gion
{
	class L3gionEditorApp : public Application
	{
	public:
		L3gionEditorApp()
			: Application("L3gion Editor")
		{
			pushLayer(new EditorLayer("Editor Layer"));
			setVsync(false);
		}

		~L3gionEditorApp()
		{
		}
	};

	Application* createApplication()
	{
		return new L3gionEditorApp();
	}
}