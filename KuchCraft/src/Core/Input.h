#pragma once

#include "Core/KeyCodes.h"

namespace KuchCraft {

	class Input
	{
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMousePositionX();
		static float GetMousePositionY();

	private:
		Input() = default;
	};

}