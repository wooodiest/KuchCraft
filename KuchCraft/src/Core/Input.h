#pragma once

#include "Core/KeyCodes.h"

#include <glm/glm.hpp>
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
		static glm::highp_dvec2 GetMousePosition();
		static double GetMousePositionX();
		static double GetMousePositionY();

	private:
		Input() = default;
	};

}