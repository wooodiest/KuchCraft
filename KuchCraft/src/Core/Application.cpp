#include "Application.h"

#include <iostream>

#include "Core/Input.h"
#include "Core/Random.h"

namespace KuchCraft {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;

		Random::Init();

		m_Window = new Window("KuchCraft", 1920, 1080, false);
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

				// Update main game loop
				if (m_WindowResized)
				{
					auto [width, height] = GetWindow().GetWindowSize();
					m_Game->OnViewportSizeChanged(width, height);
					m_WindowResized = false;
				}
				m_Game->OnUpdate(m_DeltaTime);

			}

			// Window swap buffers etc..
			m_Window->OnUpdate();
			m_Running = !m_Window->ShouldClose();
			
			if (Input::IsKeyPressed(KeyCode::Escape)) // temporary
				m_Running = false;
		}
	}

}