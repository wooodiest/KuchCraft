#pragma once

#include "Core/Window.h"
#include <glad/glad.h>

namespace KuchCraft {

	class Application
	{
	public:
		Application();
		~Application();

		void Run();

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

	private:
		float   m_DeltaTime     = 0.0f;
		float   m_LastFrametime = 0.0f;
		bool    m_Running       = true;
		bool    m_Minimized     = false;
		Window* m_Window;

	private:
		static Application* s_Instance;

	};

}
