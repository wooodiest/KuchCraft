#include "Player.h"

#include "Core/Random.h"
#include "World/World.h"

#include "Core/Core.h"
#include "Core/Log.h"

#include <iostream>

namespace KuchCraft {

	Player::Player()
	{
		OnRenderDistanceChanged(m_GraphicalSettings.RenderDistance);
		m_PrevMousePosition = Input::GetMousePosition();
	}

	Player::~Player()
	{
	}

	void Player::OnUpdate(float dt)
	{
		// Movement	
		glm::vec3 prevPosition = m_Position;

		// Mouse
		{
			auto position       = Input::GetMousePosition();
			auto positionDiff   = position - m_PrevMousePosition;
			m_PrevMousePosition = position;

			m_Rotation.x += positionDiff.x * m_PlayerMovementSettings.CameraSensitivity * dt;
			m_Rotation.y -= positionDiff.y * m_PlayerMovementSettings.CameraSensitivity * dt;

			constexpr double min_pitch = glm::radians(-89.9);
			constexpr double max_pitch = glm::radians( 89.9);

			if (m_Rotation.y < min_pitch)
				m_Rotation.y = min_pitch;

			if (m_Rotation.y > max_pitch)
				m_Rotation.y = max_pitch;
		}

		// Keyboard
		if (Input::IsKeyPressed(KeyCode::W))
		{
			float speed = Input::IsKeyPressed(KeyCode::LeftShift) ? m_PlayerMovementSettings.SprintSpeed : m_PlayerMovementSettings.Speed;
			m_Position += speed * m_Camera.GetAbsoluteFront() * dt;
		}
		if (Input::IsKeyPressed(KeyCode::S))
		{
			m_Position -= m_PlayerMovementSettings.Speed * m_Camera.GetAbsoluteFront() * dt;
		}

		if (Input::IsKeyPressed(KeyCode::A))
		{
			m_Position -= m_PlayerMovementSettings.Speed * m_Camera.GetAbsoluteRight() * dt;
		}
		if (Input::IsKeyPressed(KeyCode::D))
		{
			m_Position += m_PlayerMovementSettings.Speed * m_Camera.GetAbsoluteRight() * dt;
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			m_Position.y += m_PlayerMovementSettings.Speed * dt;
		}
		if (Input::IsKeyPressed(KeyCode::LeftControl))
		{
			m_Position.y -= m_PlayerMovementSettings.Speed * dt;
		}

		if (m_PlayerMovementSettings.CheckForCollisions)
		{
			if (CollisionCheck())
				m_Position = prevPosition;
		}
		
		m_Camera.OnUpdate(GetHeadPosition(), m_Rotation);
	}

	void Player::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(KC_BIND_EVENT_FN(Player::OnKeyPressed));

		m_Camera.OnEvent(event);
	}

	bool Player::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		switch (e.GetKeyCode())
		{
		case KeyCode::KPSubtract:
			if (m_GraphicalSettings.RenderDistance > 2)
			{
				m_GraphicalSettings.RenderDistance--;
				OnRenderDistanceChanged(m_GraphicalSettings.RenderDistance);
				World::Get().ReloadChunks();
			}
			break;
		case KeyCode::KPAdd:
			m_GraphicalSettings.RenderDistance++;
			OnRenderDistanceChanged(m_GraphicalSettings.RenderDistance);
			World::Get().ReloadChunks();
			break;

		case KeyCode::KP1: m_PlayerMovementSettings.Speed *= 1.1f; m_PlayerMovementSettings.SprintSpeed *= 1.1f; break;
		case KeyCode::KP2: m_PlayerMovementSettings.Speed *= 0.9f; m_PlayerMovementSettings.SprintSpeed *= 0.9f; break;

		default:
			break;
		}

		return false;
	}

	void Player::OnRenderDistanceChanged(int distance)
	{
		m_GraphicalSettings.RenderDistance = distance;

		float horizontalDistane = m_GraphicalSettings.RenderDistance * (chunk_size_XZ + 1) * (float)glm::sqrt(2);
		float verticalDistance = chunk_size_Y * 2.0f;

		if (horizontalDistane > verticalDistance)
			m_Camera.SetFarPlan(horizontalDistane);
		else
			m_Camera.SetFarPlan(verticalDistance);
	}

	bool Player::CollisionCheck()
	{
		glm::ivec3 position{ m_Position };

		glm::vec3 playerMinCorner{ m_Position.x - player_half_width, m_Position.y,                          m_Position.z - player_half_width };
		glm::vec3 playerMaxCorner{ m_Position.x + player_half_width, m_Position.y + player_absolute_height, m_Position.z + player_half_width };

		for (int y = 0; y <= player_absolute_height; y++)
		{
			for (int x = -player_absolute_width; x <= player_absolute_width; x++)
			{
				for (int z = -player_absolute_width; z <= player_absolute_width; z++)
				{
					glm::vec3 blockPositionMin = { position.x + x, position.y + y, position.z + z };
					Block block = World::Get().GetBlock(blockPositionMin);

					if (block.IsSolid())
					{
						constexpr float block_size = 1.0f;
						glm::vec3 blockPositionMax = { blockPositionMin.x + block_size, blockPositionMin.y + block_size, blockPositionMin.z + block_size };

						if (playerMinCorner.x < blockPositionMax.x && playerMaxCorner.x > blockPositionMin.x &&
							playerMinCorner.y < blockPositionMax.y && playerMaxCorner.y > blockPositionMin.y &&
							playerMinCorner.z < blockPositionMax.z && playerMaxCorner.z > blockPositionMin.z)
							return true;
					}
				}
			}
		}

		return false;
	}

}
