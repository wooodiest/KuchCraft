#include "kcpch.h"
#include "Core/Application.h"

#include "Core/Random.h"
#include "Core/Core.h"

namespace KuchCraft {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Random::Init();
		s_Instance = this;

		m_Window = new Window("KuchCraft", 1920, 1080, false);
		m_Window->SetEventCallback(KC_BIND_EVENT_FN(Application::OnEvent));
		m_Game   = new KuchCraft();
	}

	Application::~Application()
	{
		delete m_Game;
		delete m_Window;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			if (!m_Minimized)
			{
				// Calculate delta time
				float time = (float)glfwGetTime();
				m_DeltaTime = time - m_LastFrametime;
				m_DeltaTime = m_DeltaTime > (1.0f / 60.0f) ? (1.0f / 60.0f) : m_DeltaTime;
				m_LastFrametime = time;

				m_Game->OnUpdate(m_DeltaTime);
			}

			// Window swap buffers etc..
			m_Window->OnUpdate();
			
			if (Input::IsKeyPressed(KeyCode::Escape)) // temporary
				m_Running = false;
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(KC_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(KC_BIND_EVENT_FN(Application::OnWindowResize));

		m_Game->OnEvent(e);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		return false;
	}

}