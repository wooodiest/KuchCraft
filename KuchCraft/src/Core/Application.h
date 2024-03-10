#pragma once

#include <glad/glad.h>

#include "Core/Window.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"

#include "KuchCraft.h"

namespace KuchCraft {

	class Application
	{
	public:
		Application();
		~Application();

		void Run();

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

		void OnEvent(Event& e);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		float   m_DeltaTime     = 0.0f;
		float   m_LastFrametime = 0.0f;
		bool    m_Running       = true;
		bool    m_Minimized     = false;
		bool    m_WindowResized = false;
		Window* m_Window;

		KuchCraft* m_Game;

	private:
		static Application* s_Instance;

	};

}
