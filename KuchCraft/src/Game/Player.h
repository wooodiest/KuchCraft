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

		const Camera& GetCamera() const { return m_Camera; }

	private:
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Rotation{ 0.0f, 0.0f }; // yaw, pitch

		float m_MovementSpeed = 4.0f;
		float m_MovementSprintSpeed = 6.0f;
		float m_PlayerHeight = 1.5f;

		Camera m_Camera;

	};

}