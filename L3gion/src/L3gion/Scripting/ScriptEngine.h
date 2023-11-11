#pragma once

namespace L3gion
{
	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

	private:
		static void initMono();
		static void shutdownMono();
	};
}