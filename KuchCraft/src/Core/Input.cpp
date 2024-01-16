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

	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = Application::Get().GetWindow().GetWindow();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMousePositionX()
	{
		auto [x, y] = GetMousePosition();
		return x;
	}

	float Input::GetMousePositionY()
	{
		auto [x, y] = GetMousePosition();
		return y;
	}

}