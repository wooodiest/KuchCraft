#include "KuchCraft.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include <iostream>

namespace KuchCraft {

	KuchCraft::KuchCraft()
	{
	}

	KuchCraft::~KuchCraft()
	{

	}

	void KuchCraft::Init()
	{

	}

	void KuchCraft::OnUpdate(float dt)
	{
		m_Camera.OnUpdate(dt);

		m_Renderer.BeginScene(m_Camera);

		m_World.GetChunk({ 0.0f, 0.0f,0.0f })->Render();

		m_Renderer.EndScene();
	}

	void KuchCraft::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		Renderer::Get().OnViewportSizeChanged(width, height);
	}

}