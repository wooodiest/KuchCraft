#pragma once

#include "glm/glm.hpp"
#include "Game/Camera.h"
#include "Core/Input.h"
#include "Core/Events/Event.h"
#include "Core/Events/KeyEvent.h"

namespace KuchCraft {

	constexpr float player_height          = 1.8f;
	constexpr float player_eye_height      = 1.7f;
	constexpr int   player_absolute_height = (int)player_height + 1;
	constexpr float player_width           = 0.6f;
	constexpr float player_half_width      = player_width / 2.0f;
	constexpr int   player_absolute_width  = (int)player_width + 1;

	struct PlayerGraphicalSettings
	{
		uint32_t RenderDistance              = 8;
		uint32_t ChunksKeptInMemoryDistance  = 16;
	};

	struct PlayerMovementSettings
	{
		bool  CheckForCollisions = true;
		float CameraSensitivity  = 0.075f;
		float Speed       = 5.0f;
		float SprintSpeed = 50.0f;
		float HandRange   = 3.0f;
	};

	class Player
	{
	public:
		Player();
		~Player();

		void OnUpdate(float dt);
		void OnEvent(Event& event);

		bool OnKeyPressed(KeyPressedEvent& e);

		const Camera&           GetCamera()   const { return m_Camera;   }
		const glm::vec3&        GetPosition() const { return m_Position; }
		const glm::highp_dvec2& GetRotation() const { return m_Rotation; }

		const glm::vec3  GetHeadPosition() const { return glm::vec3(m_Position.x, m_Position.y + player_height,     m_Position.z); }
		const glm::vec3  GetEyePosition()  const { return glm::vec3(m_Position.x, m_Position.y + player_eye_height, m_Position.z); }

		void SetRenderDistance(uint32_t distance, bool reloadWorld = true);
		void SetKeptInMemoryChunksDistance(uint32_t distance, bool reloadWorld = true);

		const PlayerGraphicalSettings& GetGraphicalSettings()      { return m_GraphicalSettings;      }
		const PlayerMovementSettings&  GetPlayerMovementSettings() { return m_PlayerMovementSettings; }

	private:
		bool CollisionCheck();

	private:
		glm::vec3 m_Position{ 2000.0f, 65.0f, 2000.0f };
		glm::highp_dvec2 m_Rotation{ glm::radians(45.0f), glm::radians(-20.0f)}; // yaw, pitch
		glm::highp_dvec2 m_PrevMousePosition{ 0.0, 0.0 };

		PlayerMovementSettings  m_PlayerMovementSettings;
		PlayerGraphicalSettings m_GraphicalSettings;
		Camera m_Camera;

	};

}