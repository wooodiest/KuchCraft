#include "Application.h"

#include <iostream>

namespace KuchCraft {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;

		m_Window = new Window("KuchCraft", 1600, 900, true);
	}

	Application::~Application()
	{
		delete m_Window;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			// Renderer commands
			glClearColor(0.8f, 0.3f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Calculate delta time
			float time = glfwGetTime();
			m_DeltaTime = time - m_LastFrametime;
			m_DeltaTime = m_DeltaTime > (1.0f / 60.0f) ? (1.0f / 60.0f) : m_DeltaTime;
			m_LastFrametime = time;

			m_Window->OnUpdate();
			m_Running = !m_Window->ShouldClose();
		}
	}

}