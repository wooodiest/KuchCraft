#include "Window.h"

#include <iostream>
#include <glad/glad.h>

#include "Core/Application.h"

namespace KuchCraft {

	static void GLFWErrorCallback(int error, const char* description)
	{
		std::cout << "GLFW Error: (" << error << "): " << description << std::endl;
	}

	Window::Window(const std::string& title, uint32_t width, uint32_t height, bool vsync)
		: m_Title(title), m_Width(width), m_Height(height), m_Vsync(vsync)
	{	
		glfwInit();
		glfwSetErrorCallback(GLFWErrorCallback);

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		SetVsync(m_Vsync);
		SetCursor(false);

		// Set glfw callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{			
				if (width == 0 || height == 0)
					Application::Get().SetWindowMinimized(true);
				else
				{
					Application::Get().SetWindowMinimized(false);
					Application::Get().GetWindow().SetSize(width, height);
					Application::Get().SetWindowResized(true);
				}		
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

	void Window::SetSize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		glfwSetWindowSize(m_Window, m_Width, m_Height);
	}

	void Window::SetWidth(uint32_t width)
	{
		m_Width = width;

		glfwSetWindowSize(m_Window, m_Width, m_Height);
	}

	void Window::SetHeight(uint32_t height)
	{
		m_Height = height;

		glfwSetWindowSize(m_Window, m_Width, m_Height);
	}

	void Window::SetVsync(bool vsync)
	{
		m_Vsync = vsync;

		if (m_Vsync)
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