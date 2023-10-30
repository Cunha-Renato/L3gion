#include "lgpch.h"
#include "L3gion/Renderer/CameraController.h"

#include "L3gion/Core/Input.h"
#include "L3gion/Core/KeyCodes.h"

namespace L3gion
{
	OrthoCameraController::OrthoCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{
	}

	void OrthoCameraController::onUpdate(Timestep ts)
	{
		LG_PROFILE_FUNCTION();

		if (Input::isKeyPressed(LgKeys::LG_KEY_A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * (float)ts;
		if (Input::isKeyPressed(LgKeys::LG_KEY_D))
			m_CameraPosition.x += m_CameraTranslationSpeed * (float)ts;

		if (Input::isKeyPressed(LgKeys::LG_KEY_W))
			m_CameraPosition.y += m_CameraTranslationSpeed * (float)ts;
		if (Input::isKeyPressed(LgKeys::LG_KEY_S))
			m_CameraPosition.y -= m_CameraTranslationSpeed * (float)ts;

		if (m_Rotation)
		{
			if (Input::isKeyPressed(LgKeys::LG_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * (float)ts;
			if (Input::isKeyPressed(LgKeys::LG_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * (float)ts;
		
			m_Camera.setRotation(m_CameraRotation);
		}

		m_Camera.setPosition(m_CameraPosition);
	}

	void OrthoCameraController::calculateView()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel};
		m_Camera.setProjetion(m_Bounds.left, m_Bounds.right, m_Bounds.bottom, m_Bounds.top);
	}

	void OrthoCameraController::resize(float width, float height)
	{
		m_AspectRatio = width / height;
		calculateView();
	}

	void OrthoCameraController::onEvent(Event& e)
	{
		LG_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(BIND_EVENT_FN(OrthoCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthoCameraController::onWindowResized));
	}
	bool OrthoCameraController::onMouseScrolled(MouseScrollEvent& e)
	{
		LG_PROFILE_FUNCTION();

		m_ZoomLevel -= e.getYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

		calculateView();

		return false;
	}
	bool OrthoCameraController::onWindowResized(WindowResizeEvent& e)
	{
		LG_PROFILE_FUNCTION();

		resize((float)e.getWidth(), (float)e.getHeight());
		calculateView();
		return false;
	}
}