#pragma once

#include "Core/KeyCodes.h"

#include <utility>

namespace KuchCraft {

	class Input
	{
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMousePositionX();
		static float GetMousePositionY();

	private:
		Input() = default;
	};

}