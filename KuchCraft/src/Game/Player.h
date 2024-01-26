#pragma once

#include "glm/glm.hpp"
#include "Renderer/Camera.h"

namespace KuchCraft {

	class Player
	{
	public:
		Player();
		~Player();

		void OnUpdate(float dt);

		const Camera&    GetCamera()   const { return m_Camera; }
		const glm::vec3  GetPosition() const { return m_Position; }
		const glm::vec2& GetRotation() const { return m_Rotation; }

	private:
		glm::vec3 m_Position{ 0.0f, 70.0f, 0.0f };
		glm::vec2 m_Rotation{ 0.0f, 0.0f }; // yaw, pitch

		float m_MovementSpeed = 10.0f;
		float m_MovementSprintSpeed = 20.0f;
		float m_PlayerHeight = 1.5f;

		Camera m_Camera;

	};

}