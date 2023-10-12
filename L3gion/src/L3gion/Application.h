#pragma once

#include "Core.h"

namespace L3gion
{
	class L3GION_API Application
	{
		public:
			Application();
			virtual ~Application();

			void run();
	};

	// To be defined in CLIENT
	Application* createApplication();
}