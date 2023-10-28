#pragma once

namespace L3gion
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void init() = 0;
		virtual void swapBuffers() = 0;

		static scope<GraphicsContext> create(void* window);
	};
}