#include "kcpch.h"
#include "Core/Window.h"

#include "Core/Application.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

#include <glad/glad.h>

namespace KuchCraft {

	static void GLFWErrorCallback(int error, const char* description)
	{
		KC_ERROR("GLFW: {0} : {1}", error, description);
	}

	Window::Window(const std::string& title, uint32_t width, uint32_t height, bool vsync)
	{	
		m_Data.Title  = title;
		m_Data.Width  = width;
		m_Data.Height = height;
		m_Data.Vsync  = vsync;

		// GLFW
		{
			int succes = glfwInit();
			if (!succes)
			{
				KC_ERROR("GLFW init error");
			}
		}

		glfwSetErrorCallback(GLFWErrorCallback);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), glfwGetPrimaryMonitor(), nullptr);
		glfwMakeContextCurrent(m_Window);

		// Glad
		{
			int succes = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			if (!succes)
			{
				KC_ERROR("Glad init error");
			}
		}

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVsync(m_Data.Vsync);
		SetCursor(false);

		// Set glfw callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{		
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);						
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), false);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), true);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(static_cast<KeyCode>(keycode));
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(static_cast<MouseCode>(button));
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}

	void Window::SetVsync(bool vsync)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(m_Window);
		data.Vsync = vsync;

		if (m_Data.Vsync)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}

	void Window::SetCursor(bool visible)
	{
		if (visible)
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

}