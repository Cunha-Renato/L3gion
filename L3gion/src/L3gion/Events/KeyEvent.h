#pragma once

#include "Event.h"

namespace L3gion
{
	class L3GION_API KeyEvent : public Event
	{
	public:
		inline int getKeyCode() { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EC_Keyboard | EC_Input)

	protected:
		KeyEvent(int keyCode)
			: m_KeyCode(keyCode) {}

		int m_KeyCode;
	};

//----------------- KEY_PRESSED -------------------------

	class L3GION_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keyCode, int repeatCount)
			: KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

		inline int getRepeatCount() const { return m_RepeatCount; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

//----------------- KEY_RELEASED -------------------------

	class L3GION_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keyCode)
			: KeyEvent(keyCode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

//----------------- KEY_TYPED -------------------------

	class L3GION_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keyCode)
			: KeyEvent(keyCode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}

FORMAT_TOSTR_IMPL(L3gion::KeyEvent);
FORMAT_TOSTR_IMPL(L3gion::KeyPressedEvent);
FORMAT_TOSTR_IMPL(L3gion::KeyReleasedEvent);