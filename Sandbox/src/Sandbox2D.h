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
	bool onKeyPressed(L3gion::Event& e);

private:
	L3gion::OrthoCameraController m_CameraController;

	L3gion::ref<L3gion::Texture2D> m_SpriteSheetTexture;
	L3gion::ref<L3gion::SubTexture2D> m_Checkerboard;
	L3gion::ref<L3gion::SubTexture2D> m_Tree;

	glm::vec4 m_Color = { 0.8f, 0.2f, 0.3f, 1.0f };

	double m_Timesep = 0;
};

