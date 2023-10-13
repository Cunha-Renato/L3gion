#pragma once

#include "Event.h"

namespace L3gion
{
	class L3GION_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float getX() const { return m_MouseX; }
		inline float getY() const { return m_MouseY; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input);

	private:
		float m_MouseX, m_MouseY;
	};

//---------------------- SCROLL_EVENT ------------------------

	class L3GION_API MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float getXOffset() const { return m_XOffset; }
		inline float getYOffset() const { return m_YOffset; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseScrollEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input);

	private:
		float m_XOffset, m_YOffset;
	};

//------------------------ BUTTON_EVENT ------------------------
	
	class L3GION_API MouseButtonEvent : public Event
	{
	public:
		inline int getMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input);

	protected:
		MouseButtonEvent(int button)
			: m_Button(button) {}
		int m_Button;
	};

//------------------------ BUTTON_PRESSED_EVENT ------------------------

	class L3GION_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

//------------------------ BUTTON_RELEASE_EVENT ------------------------

	class L3GION_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}

FORMAT_TOSTR_IMPL(L3gion::MouseMovedEvent);
FORMAT_TOSTR_IMPL(L3gion::MouseScrollEvent);
FORMAT_TOSTR_IMPL(L3gion::MouseButtonEvent);
FORMAT_TOSTR_IMPL(L3gion::MouseButtonPressedEvent);
FORMAT_TOSTR_IMPL(L3gion::MouseButtonReleasedEvent);