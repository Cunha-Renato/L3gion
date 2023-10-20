#pragma once

#include <glm/glm.hpp>

namespace L3gion
{
	class OrthoCamera
	{
	public:
		OrthoCamera(float left, float right, float botton, float top);

		const glm::vec3& getPosition() { return m_Position; }
		const float getRotation() { return m_Rotation; }

		void setPosition(const glm::vec3& position) 
		{ 
			m_Position = position;
			updateViewMatrix();
		}
		void setRotation(float rotation)
		{ 
			m_Rotation = rotation; 
			updateViewMatrix();
		}

		const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		void updateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

		glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		float m_Rotation = 0.0f;
	};
}