#pragma once

#include "glm/glm.hpp"
#include "Renderer/Camera.h"
#include "Core/Input.h"

namespace KuchCraft {

	struct PlayerGraphicalSettings
	{
		uint32_t RenderDistance              = 10;
		uint32_t ChunksKeptInMemoryDistance  = 20;
	};

	struct PlayerMovementSettings
	{
		float Speed       = 10.0f;
		float SprintSpeed = 50.0f;
		float Height      = 1.5f;
		float HandRange   = 3.0f;
	};

	class Player
	{
	public:
		Player();
		~Player();

		void OnUpdate(float dt);

		const Camera&    GetCamera()   const { return m_Camera; }
		const glm::vec3  GetPosition() const { return m_Position; }
		const glm::vec2& GetRotation() const { return m_Rotation; }

		void SetRenderDistance(uint32_t distance)             { m_GraphicalSettings.RenderDistance             = distance; }
		void SetKeptInMemoryChunksDistance(uint32_t distance) { m_GraphicalSettings.ChunksKeptInMemoryDistance = distance; }

		PlayerGraphicalSettings& GetGraphicalSettings()      { return m_GraphicalSettings;      }
		PlayerMovementSettings&  GetPlayerMovementSettings() { return m_PlayerMovementSettings; }

	private:
		glm::vec3 m_Position{ 2000.0f, 90.0f, 2000.0f };
		glm::vec2 m_Rotation{ glm::radians(45.0f), glm::radians(-20.0f)}; // yaw, pitch

		PlayerMovementSettings  m_PlayerMovementSettings;
		PlayerGraphicalSettings m_GraphicalSettings;
		Camera m_Camera;

		InputMouseHandler m_LeftMouseButtonClick{ MouseCode::ButtonLeft, true, 0.5f };
	};

}