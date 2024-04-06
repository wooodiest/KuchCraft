#include "kcpch.h"
#include "KuchCraft.h"

#include "Renderer/Renderer.h"

namespace KuchCraft {

	KuchCraftSettings KuchCraft::s_Settings;

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
		Renderer::BeginFrame();

		if (m_World)
		{
			m_World->OnUpdate(dt);
			m_QuitStatus = m_World->GetQuitStatus();
		}

		Renderer::EndFrame(m_World ? &m_World->GetCamera() : nullptr);

		if (m_QuitStatus)
		{
			delete m_World;
			m_World = nullptr;
		}
	}

	void KuchCraft::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(KC_BIND_EVENT_FN(KuchCraft::OnWindowResize));

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

	bool KuchCraft::OnWindowResize(WindowResizeEvent& e)
	{
		Renderer::OnViewportSizeChanged(e.GetWidth(), e.GetHeight());
		return false;
	}

}