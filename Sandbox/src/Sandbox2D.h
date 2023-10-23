#pragma once

#include "L3gion.h"
#include "L3gion/Renderer/Texture.h"

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
	L3gion::ref<L3gion::Shader> m_Shader;
	L3gion::ref<L3gion::VertexArray> m_VertexArray;

	L3gion::ref<L3gion::Texture2D> m_Texture;

	glm::vec4 m_Color = { 0.8f, 0.2f, 0.3f, 1.0f };
};

