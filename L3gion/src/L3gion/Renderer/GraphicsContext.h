#pragma once

namespace L3gion
{
	class GraphicsContext
	{
	public:
		virtual void init() = 0;
		virtual void swapBuffers() = 0;
	};
}