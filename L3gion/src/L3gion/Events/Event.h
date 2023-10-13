#pragma once

#include "L3gion/Core.h"
#include "L3gion/Log.h"

namespace L3gion
{
	//TODO: Events are currently blocking. Implement a buffer sistem

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowMoved,
		AppTick, AppUpdate, AppRender, //TODO: Maybe not necessary
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EC_Application = BIT(0),
		EC_Input       = BIT(1),
		EC_Keyboard    = BIT(2),
		EC_Mouse       = BIT(3),
		EC_MouseButton = BIT(4)
	};
	

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
	                           virtual EventType getEventType() const override { return getStaticType(); }\
                               virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

	class L3GION_API Event
	{
		friend class EventDispatcher;
		inline friend std::ostream& operator<<(std::ostream& os, const Event& e)
		{
  			return os << e.toString();
		}

	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		inline bool isInCategory(EventCategory category)
		{
			return getCategoryFlags() & category;
		}

	protected:
		bool m_Handled = false;
		
	};

//------------------------ DISPATCHER ------------------------

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	
	public:
		EventDispatcher(Event& e)
			: m_Event(e) {}

		template<typename T>
		bool dispatch(EventFn<T> func)
		{
			if (m_Event.getEventType() == T::getStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};
}
FORMAT_TOSTR_IMPL(L3gion::Event);

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)