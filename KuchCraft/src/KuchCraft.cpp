#include "KuchCraft.h"

#include "World/World.h"
#include "Renderer/Renderer.h"

namespace KuchCraft {

	KuchCraft::KuchCraft()
	{
		Renderer::Init();
		World::Init();
	}

	KuchCraft::~KuchCraft()
	{
		World::Shutdown();
		Renderer::ShutDown();
	}

	void KuchCraft::OnUpdate(float dt)
	{
		m_Player.OnUpdate(dt);
		World::OnUpdate(dt);

		Renderer::ResetStats();
		Renderer::BeginScene(m_Player.GetCamera());	
		World::Render(m_Player.GetPosition(), m_Player.GetRotation(), 0.0f); // TODO: Get
		Renderer::EndScene();
	}

	void KuchCraft::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		Renderer::OnViewportSizeChanged(width, height);
	}

}