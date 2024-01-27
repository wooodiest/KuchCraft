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
		static glm::vec2 GetMousePosition();
		static float GetMousePositionX();
		static float GetMousePositionY();

	private:
		Input() = default;
	};

	class InputKeyboardHandler
	{
	public:
		InputKeyboardHandler(KeyCode key, bool repeat = false, float time = 0.5f);
		void OnUpdate(float dt);
		bool PerformAction() const { return m_PerformAction; }

	private:
		KeyCode m_Key;
		bool    m_Repeat;
		float   m_Time;
		bool    m_PerformAction = false;
		float   m_CurrentTime = 0.0f;
	};

	class InputMouseHandler
	{
	public:
		InputMouseHandler(MouseCode button, bool repeat = false, float time = 0.5f);
		void OnUpdate(float dt);
		bool PerformAction() const { return m_PerformAction; }

	private:
		MouseCode m_Button;
		bool    m_Repeat;
		float   m_Time;
		bool    m_PerformAction = false;
		float   m_CurrentTime = 0.0f;
	};

}