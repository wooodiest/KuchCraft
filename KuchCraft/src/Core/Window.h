#pragma once

#include <GLFW/glfw3.h>
#include "Core/Events/Event.h"

namespace KuchCraft {

	class Window
	{
	public:
		Window(const std::string& title, uint32_t width, uint32_t height, bool vsync);
		~Window();

		using EventCallbackFn = std::function<void(Event&)>;
		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

		void OnUpdate();
		bool ShouldClose();

		uint32_t GetWidth()  const { return m_Data.Width;  }
		uint32_t GetHeight() const { return m_Data.Height; }
		std::pair<uint32_t, uint32_t> GetWindowSize() const { return { m_Data.Width, m_Data.Height }; }
		bool     IsVsync()   const { return m_Data.Vsync;  }

		GLFWwindow* GetWindow() { return m_Window; }

		void SetVsync(bool vsync);
		void SetCursor(bool visible);

	private:
		// Window data
		GLFWwindow* m_Window;
		struct WindowData
		{
			std::string Title;
			uint32_t    Width, Height;
			bool        Vsync;
			EventCallbackFn EventCallback;
		};
		WindowData m_Data;

	};

}