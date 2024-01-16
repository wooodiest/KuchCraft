#pragma once

#include <string>
#include <GLFW/glfw3.h>

namespace KuchCraft {

	class Window
	{
	public:
		Window(const std::string& title, uint32_t width, uint32_t height, bool vsync);
		~Window();

		void OnUpdate();

		uint32_t GetWidth()  const { return m_Width;  }
		uint32_t GetHeight() const { return m_Height; }
		bool     IsVsync()   const { return m_Vsync;  }

		GLFWwindow* GetWindow() { return m_Window; }

		void SetSize(uint32_t width, uint32_t height);
		void SetWidth(uint32_t width);
		void SetHeight(uint32_t height);
		void SetVsync(bool vsync);

	private:
		// Window data
		std::string m_Title;
		uint32_t    m_Width, m_Height;
		bool        m_Vsync;
		GLFWwindow* m_Window;
	};

}