#pragma once

#include "Event.h"

namespace L3gion
{
	class L3GION_API WindowResizeEvent : public Event
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

	class L3GION_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EC_Application)
	};

//---------------------- APP_TICK ------------------------

	class L3GION_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}
		
		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EC_Application)
	};

//---------------------- APP_UPDATE ------------------------

	class L3GION_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EC_Application)
	};

//---------------------- APP_RENDER ------------------------
	
	class L3GION_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EC_Application)

	};
}

FORMAT_TOSTR_IMPL(L3gion::WindowResizeEvent);
FORMAT_TOSTR_IMPL(L3gion::WindowCloseEvent);
FORMAT_TOSTR_IMPL(L3gion::AppTickEvent);
FORMAT_TOSTR_IMPL(L3gion::AppUpdateEvent);
FORMAT_TOSTR_IMPL(L3gion::AppRenderEvent);