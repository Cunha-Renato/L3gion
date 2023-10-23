#pragma once

#include "L3gion.h"

class Sandbox2D : public L3gion::Layer
{
public:
	Sandbox2D(const std::string& name);
	~Sandbox2D() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	virtual void onUpdate(L3gion::Timestep& ts) override;
	virtual void onImGuiRender() override;
	virtual void onEvent(L3gion::Event& e) override;

private:
	L3gion::OrthoCameraController m_CameraController;
	
	// Temporary
	L3gion::ShaderLib m_ShaderLib;
	L3gion::ref<L3gion::VertexArray> m_VertexArray;

	glm::vec4 m_Color = { 0.2f, 0.3f, 0.8f, 1.0f };
};

