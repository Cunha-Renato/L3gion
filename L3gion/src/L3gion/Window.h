#pragma once

#include "lgpch.h"
#include "L3gion/Core.h"
#include "L3gion/Events/Event.h"

namespace L3gion
{
	struct WindowProps
	{
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProps(const std::string& Title = "L3gion Engine",
			unsigned int Width = 1280,
			unsigned int Height = 720)
			: title(Title), width(Width), height(Height)
		{
		}
	};

	// Interface representing a desktop window
	class L3GION_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		// Window attributes
		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};
}