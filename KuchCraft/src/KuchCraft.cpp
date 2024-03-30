#include "kcpch.h"
#include "KuchCraft.h"

#include "Renderer/Renderer.h"

namespace KuchCraft {

	KuchCraftSettings KuchCraft::s_Settings;

	KuchCraft::KuchCraft()
	{
		KC_PROFILE_FUNCTION();

		Renderer::Init();
		LoadWorld("Example");
	}

	KuchCraft::~KuchCraft()
	{
		KC_PROFILE_FUNCTION();

		if (m_World)
			delete m_World;
		
		Renderer::ShutDown();
	}

	void KuchCraft::OnUpdate(float dt)
	{
		KC_PROFILE_FUNCTION();

		Renderer::BeginFrame();

		if (m_World)
		{
			m_World->OnUpdate(dt);

			// Check to safety exit from world
			if (m_World->GetQuitStatus()) // TODO: Move this outside end frame so it can render last frame befor quiting
			{
				delete m_World;
				m_World = nullptr;
			}
		}

		Renderer::EndFrame(m_World ? &m_World->GetCamera() : nullptr);
	}

	void KuchCraft::OnEvent(Event& event)
	{
		if (m_World)
			m_World->OnEvent(event);
	}

	void KuchCraft::LoadWorld(const std::string& path)
	{
		KC_PROFILE_FUNCTION();

		if (m_World)
		{
			delete m_World;
			m_World = nullptr;
		}
		m_World = new World(path);	
	}

	void KuchCraft::SetRenderDistance(uint32_t distance)
	{
		s_Settings.RenderDistance = distance;

		if (m_World)
			m_World->ReloadChunks();
	}

	void KuchCraft::SetKeptInMemoryChunksDistance(uint32_t distance)
	{
		s_Settings.ChunksKeptInMemoryDistance = distance;
		if (m_World)
			m_World->ReloadChunks();
	}

}