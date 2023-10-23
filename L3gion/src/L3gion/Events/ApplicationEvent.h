#pragma once

#include "L3gion/Events/Event.h"

namespace L3gion
{
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		inline unsigned int getWidth() const { return m_Width; }
		inline unsigned int getHeight() const { return m_Height; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EC_Application)
	private:
		unsigned int m_Width, m_Height;
	};

//---------------------- CLOSE ------------------------

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EC_Application)
	};

//---------------------- APP_TICK ------------------------

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;
		
		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EC_Application)
	};

//---------------------- APP_UPDATE ------------------------

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EC_Application)
	};

//---------------------- APP_RENDER ------------------------
	
	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EC_Application)

	};
}

FORMAT_TOSTR_IMPL(L3gion::WindowResizeEvent);
FORMAT_TOSTR_IMPL(L3gion::WindowCloseEvent);
FORMAT_TOSTR_IMPL(L3gion::AppTickEvent);
FORMAT_TOSTR_IMPL(L3gion::AppUpdateEvent);
FORMAT_TOSTR_IMPL(L3gion::AppRenderEvent);