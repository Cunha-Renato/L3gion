#pragma once

#include "L3gion/Events/Event.h"

namespace L3gion
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(glm::vec2 position)
			: m_Position(position) {}
		MouseMovedEvent(float x, float y)
		{
			m_Position = glm::vec2(x, y);
		}

		inline float getX() const { return m_Position.x; }
		inline float getY() const { return m_Position.y; }
		inline glm::vec2 getPosition() const { return m_Position; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_Position.x << ", " << m_Position.y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input);

	private:
		glm::vec2 m_Position;
	};

//---------------------- SCROLL_EVENT ------------------------

	class MouseScrollEvent : public Event
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
	
	class MouseButtonEvent : public Event
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

	class MouseButtonPressedEvent : public MouseButtonEvent
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

	class MouseButtonReleasedEvent : public MouseButtonEvent
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