#include "lgpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace L3gion
{
	SceneCamera::SceneCamera() 
	{
		recalculateProjection();
	}

	void SceneCamera::setPerspective(float verticalFov, float nearClip, float farClip)
	{
		setProjectionType(ProjectionType::Perspective);

		m_PerspectiveVerticalFov = verticalFov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		recalculateProjection();
	}

	void SceneCamera::setOrthographic(float size, float nearClip, float farClip)
	{
		setProjectionType(ProjectionType::Orthographic);

		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		recalculateProjection();
	}

	void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		
		recalculateProjection();
	}

	void SceneCamera::recalculateProjection()
	{
		switch (m_ProjectionType)
		{
			case L3gion::SceneCamera::ProjectionType::Perspective:
			{
				m_Projection = glm::perspective(m_PerspectiveVerticalFov, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
				break;
			}
			case L3gion::SceneCamera::ProjectionType::Orthographic:
			{
				float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
				float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
				float orthoBottom = -m_OrthographicSize * 0.5f;
				float orthoTop = m_OrthographicSize * 0.5f;

				m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
				break;
			}
			default:
				break;
		}
	}

}