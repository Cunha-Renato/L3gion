#pragma once

#include "lgpch.h"
#include "L3gion/Core/Core.h"
#include "L3gion/Events/Event.h"

namespace L3gion
{
	struct WindowProps
	{
		std::string title;
		uint32_t width;
		uint32_t height;

		WindowProps(const std::string& Title = "L3gion Engine",
			uint32_t Width = 1280,
			uint32_t Height = 720)
			: title(Title), width(Width), height(Height)
		{
		}
	};

	// Interface representing a desktop window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		// Window attributes
		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;

		static scope<Window> create(const WindowProps& props = WindowProps());
	};
}