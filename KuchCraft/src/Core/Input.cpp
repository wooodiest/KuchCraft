#include "Input.h"

#include <GLFW/glfw3.h>

#include "Core/Application.h"

namespace KuchCraft {

	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto window = Application::Get().GetWindow().GetWindow();
		auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = Application::Get().GetWindow().GetWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec<2, double> Input::GetMousePosition()
	{
		auto window = Application::Get().GetWindow().GetWindow();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { xpos, ypos };
	}

	float Input::GetMousePositionX()
	{
		auto position = GetMousePosition();
		return position.x;
	}

	float Input::GetMousePositionY()
	{
		auto position = GetMousePosition();
		return position.y;
	}

	InputKeyboardHandler::InputKeyboardHandler(KeyCode key, bool repeat, float time)
		: m_Key(key), m_Time(time), m_Repeat(repeat)
	{
	}

	void InputKeyboardHandler::OnUpdate(float dt)
	{
		if (Input::IsKeyPressed(m_Key))
		{
			if (m_CurrentTime == 0.0f)
				m_PerformAction = true;
			else
				m_PerformAction = false;
			
			m_CurrentTime += dt;

			if (m_Repeat && m_CurrentTime > m_Time)
				m_CurrentTime = 0.0f;		

		}
		else
			m_CurrentTime = 0.0f;
	}

	InputMouseHandler::InputMouseHandler(MouseCode button, bool repeat, float time)
		: m_Button(button), m_Time(time), m_Repeat(repeat)
	{
	}

	void InputMouseHandler::OnUpdate(float dt)
	{
		if (Input::IsMouseButtonPressed(m_Button))
		{
			if (m_CurrentTime == 0.0f)
				m_PerformAction = true;
			else
				m_PerformAction = false;

			m_CurrentTime += dt;

			if (m_Repeat && m_CurrentTime > m_Time)
				m_CurrentTime = 0.0f;

		}
		else
			m_CurrentTime = 0.0f;
	}

}