#pragma once

#include "L3gion/Renderer/Camera.h"
#include "L3gion/Core/Timestep.h"

#include "L3gion/Events/ApplicationEvent.h"
#include "L3gion/Events/MouseEvent.h"

namespace L3gion
{
	class OrthoCameraController
	{
	public:
		OrthoCameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		OrthoCamera& getCamera() { return m_Camera; }
		const OrthoCamera& getCamera() const { return m_Camera; }
	private:
		bool onMouseScrolled(MouseScrollEvent& e);
		bool onWindowResized(WindowResizeEvent& e);

	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthoCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 3.0f;
		float m_CameraRotationSpeed = 180.0f;
	};
}