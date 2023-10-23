#include "lgpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace L3gion
{
	OrthoCamera::OrthoCamera(float left, float right, float botton, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, botton, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthoCamera::setProjetion(float left, float right, float botton, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, botton, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthoCamera::updateViewMatrix()
	{
		glm::mat4 transfom = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		m_ViewMatrix = glm::inverse(transfom);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}