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

	void OrthoCameraController::OnUpdate(Timestep ts)
	{
		if (Input::isKeyPressed(LG_KEY_A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * (float)ts;
		if (Input::isKeyPressed(LG_KEY_D))
			m_CameraPosition.x += m_CameraTranslationSpeed * (float)ts;

		if (Input::isKeyPressed(LG_KEY_W))
			m_CameraPosition.y += m_CameraTranslationSpeed * (float)ts;
		if (Input::isKeyPressed(LG_KEY_S))
			m_CameraPosition.y -= m_CameraTranslationSpeed * (float)ts;

		if (m_Rotation)
		{
			if (Input::isKeyPressed(LG_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * (float)ts;
			if (Input::isKeyPressed(LG_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * (float)ts;
		
			m_Camera.setRotation(m_CameraRotation);
		}

		m_Camera.setPosition(m_CameraPosition);
	}

	void OrthoCameraController::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(BIND_EVENT_FN(OrthoCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthoCameraController::onWindowResized));
	}

	bool OrthoCameraController::onMouseScrolled(MouseScrollEvent& e)
	{
		m_ZoomLevel -= e.getYOffset() * 0.25f;

		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

		m_Camera.setProjetion(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}
	bool OrthoCameraController::onWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.getWidth() / (float)e.getHeight();
		m_Camera.setProjetion(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}