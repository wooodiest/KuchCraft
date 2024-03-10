#include "KuchCraft.h"

#include <iostream>

#include "Renderer/Renderer.h"

namespace KuchCraft {

	KuchCraft::KuchCraft()
	{
		Renderer::Init();
		LoadWorld("Example");
	}

	KuchCraft::~KuchCraft()
	{
		if (m_World)
			delete m_World;
		
		Renderer::ShutDown();
	}

	void KuchCraft::OnUpdate(float dt)
	{
		Renderer::OnUpdate(dt);
		if (m_World)
		{
			m_World->OnUpdate(dt);
			m_World->Render();

			// Check to safety exit from world
			if (m_World->GetQuitStatus())
			{
				delete m_World;
				m_World = nullptr;
			}
		}
	}

	void KuchCraft::OnEvent(Event& event)
	{
		if (m_World)
			m_World->OnEvent(event);
	}

	void KuchCraft::LoadWorld(const std::string& path)
	{
		if (m_World)
		{
			delete m_World;
			m_World = nullptr;
		}
		m_World = new World(path);	
	}

}