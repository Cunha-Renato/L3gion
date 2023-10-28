#pragma once

#include "L3gion/Renderer/Camera.h"
#include "L3gion/Core/Timestep.h"

#include "L3gion/Events/ApplicationEvent.h"
#include "L3gion/Events/MouseEvent.h"

namespace L3gion
{
	class OrthoCameraController
	{
		struct OrthoCameraBounds
		{
			float left, right;
			float bottom, top;

			float getWidth() { return right - left; }
			float getHeight() { return top - bottom; }
		};

	public:
		OrthoCameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		void resize(float width, float height);

		OrthoCamera& getCamera() { return m_Camera; }
		const OrthoCamera& getCamera() const { return m_Camera; }

		float getZoomLevel() const { return m_ZoomLevel; }
		void setZoomLevel(float level) 
		{ 
			m_ZoomLevel = level; 
			calculateView();
		}

	private:
		bool onMouseScrolled(MouseScrollEvent& e);
		bool onWindowResized(WindowResizeEvent& e);

	private:
		void calculateView();
		
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthoCameraBounds m_Bounds;
		OrthoCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 3.0f;
		float m_CameraRotationSpeed = 180.0f;
	};
}