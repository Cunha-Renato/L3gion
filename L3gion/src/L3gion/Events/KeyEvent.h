#pragma once

#include "L3gion/Events/Event.h"
#include "L3gion/Core/KeyCodes.h"

namespace L3gion
{
	class KeyEvent : public Event
	{
	public:
		inline LgKeys getKeyCode() { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EC_Keyboard | EC_Input)

	protected:
		KeyEvent(LgKeys keyCode)
			: m_KeyCode(keyCode) {}

		LgKeys m_KeyCode;
	};

//----------------- KEY_PRESSED -------------------------

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(LgKeys keyCode, int repeatCount)
			: KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

		inline int getRepeatCount() const { return m_RepeatCount; }
		inline LgKeys getKeyCode() { return m_KeyCode; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << (int)m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

//----------------- KEY_RELEASED -------------------------

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(LgKeys keyCode)
			: KeyEvent(keyCode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << (int)m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

//----------------- KEY_TYPED -------------------------

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(unsigned int keyCode) // TODO: this is wrong
			: KeyEvent((LgKeys)0) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << (unsigned int)m_KeyCode;

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}

FORMAT_TOSTR_IMPL(L3gion::KeyEvent);
FORMAT_TOSTR_IMPL(L3gion::KeyPressedEvent);
FORMAT_TOSTR_IMPL(L3gion::KeyReleasedEvent);