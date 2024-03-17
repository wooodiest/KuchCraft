#include "Player.h"

#include "Core/Random.h"
#include "World/World.h"

#include "Core/Core.h"
#include "Core/Log.h"

#include "Renderer/Renderer.h"

#include <glm/gtc/epsilon.hpp>
#include <iostream>

namespace KuchCraft {

	Player::Player()
	{
		SetRenderDistance(m_GraphicalSettings.RenderDistance, false);
		m_PrevMousePosition = Input::GetMousePosition();
	}

	Player::~Player()
	{
	}

	void Player::OnUpdate(float dt)
	{
		// Movement	
		// Mouse
		{
			glm::vec2 position     = Input::GetMousePosition();
			glm::vec2 positionDiff = position - m_PrevMousePosition;
			m_PrevMousePosition    = position;

			m_Rotation.x += positionDiff.x * m_MovementSettings.CameraSensitivity * 0.001f;
			m_Rotation.y -= positionDiff.y * m_MovementSettings.CameraSensitivity * 0.001f;

			constexpr float min_pitch = glm::radians(-89.9f);
			constexpr float max_pitch = glm::radians( 89.9f);

			if (m_Rotation.y < min_pitch)
				m_Rotation.y = min_pitch;

			if (m_Rotation.y > max_pitch)
				m_Rotation.y = max_pitch;
		}

		// Keyboard
		glm::vec3 prevPosition = m_Position;

		if (Input::IsKeyPressed(KeyCode::W))
			m_MovementVector += m_Camera.GetAbsoluteFront();	
		if (Input::IsKeyPressed(KeyCode::S))
			m_MovementVector -= m_Camera.GetAbsoluteFront();
		
		if (Input::IsKeyPressed(KeyCode::A))
			m_MovementVector -= m_Camera.GetAbsoluteRight();
		if (Input::IsKeyPressed(KeyCode::D))
			m_MovementVector += m_Camera.GetAbsoluteRight();
		
		if (Input::IsKeyPressed(KeyCode::Space))
			m_MovementVector += m_Camera.GetUp();
		if (Input::IsKeyPressed(KeyCode::LeftControl))
			m_MovementVector -= m_Camera.GetUp();
		
		if (glm::length(m_MovementVector) > 0.0f)
			m_MovementVector = glm::normalize(m_MovementVector);

		glm::vec3 newPosition = m_Position + m_MovementVector * m_MovementSettings.Speed * dt;
		if (m_MovementSettings.CheckForCollisions)
		{
			glm::vec3 collisionNormal{ 0.0f };
			if (CollisionCheck(newPosition, collisionNormal) && glm::length(collisionNormal) > 0.0f)
			{
				constexpr int maxCorrectionAttempts = 25;
				constexpr float epsilon = 0.001f;

				glm::vec3 correctedPosition = newPosition;
				if (CollisionCheck(correctedPosition, collisionNormal) && glm::length(collisionNormal) > 0.0f)
				{
					int attempts = 0;
					correctedPosition -= collisionNormal * epsilon;
					while (CollisionCheck(correctedPosition, collisionNormal) && attempts < maxCorrectionAttempts && glm::length(collisionNormal) > 0.0f)
					{
						correctedPosition -= collisionNormal * epsilon;
						attempts++;
					}
					newPosition = correctedPosition;
				}
			}		
		}
		m_Position = newPosition;

		m_MovementVector = { 0.0f, 0.0f, 0.0f };
		m_Camera.OnUpdate(GetEyePosition(), m_Rotation);
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
			case KeyCode::CapsLock: m_MovementSettings.CheckForCollisions = !m_MovementSettings.CheckForCollisions; return false;
			default: return false;
		}
	}

	void Player::SetRenderDistance(uint32_t distance, bool reloadWorld)
	{
		m_GraphicalSettings.RenderDistance = distance;
		if (reloadWorld)
			World::Get().ReloadChunks();

		// Set far plan to camera
		float horizontalDistane = m_GraphicalSettings.RenderDistance * (chunk_size_XZ + 1) * (float)glm::sqrt(2.0f);
		float verticalDistance  = chunk_size_Y * 2.0f;

		if (horizontalDistane > verticalDistance)
			m_Camera.SetFarPlan(horizontalDistane);
		else
			m_Camera.SetFarPlan(verticalDistance);
	}

	void Player::SetKeptInMemoryChunksDistance(uint32_t distance, bool reloadWorld)
	{
		m_GraphicalSettings.ChunksKeptInMemoryDistance = distance;
		if (reloadWorld)
			World::Get().ReloadChunks();
	}

	bool Player::CollisionCheck(const glm::vec3& newPosition, glm::vec3& collisionNormal)
	{
		bool colided = false;

		const glm::ivec3 position{ newPosition };
		const glm::vec3  playerMinCorner{ newPosition.x - player_half_width, newPosition.y,       newPosition.z - player_half_width };
		const glm::vec3  playerMaxCorner{ newPosition.x + player_half_width, GetHeadPosition().y, newPosition.z + player_half_width};

		for (int y = 0; y <= player_absolute_height; y++)
		{
			for (int x = -player_absolute_width; x <= player_absolute_width; x++)
			{
				for (int z = -player_absolute_width; z <= player_absolute_width; z++)
				{
					const glm::vec3 blockPositionMin = { position.x + x, position.y + y, position.z + z };
					const Block block = World::Get().GetBlock(blockPositionMin);

					if (block.IsSolid())
					{
						constexpr float block_size = 1.0f;
						const glm::vec3 blockPositionMax = { blockPositionMin.x + block_size, blockPositionMin.y + block_size, blockPositionMin.z + block_size };

						if (playerMinCorner.x < blockPositionMax.x && playerMaxCorner.x > blockPositionMin.x &&
							playerMinCorner.y < blockPositionMax.y && playerMaxCorner.y > blockPositionMin.y &&
							playerMinCorner.z < blockPositionMax.z && playerMaxCorner.z > blockPositionMin.z)

						{
							colided = true;

							const float xOverlap = glm::min(playerMaxCorner.x, blockPositionMax.x) - glm::max(playerMinCorner.x, blockPositionMin.x);
							const float yOverlap = glm::min(playerMaxCorner.y, blockPositionMax.y) - glm::max(playerMinCorner.y, blockPositionMin.y);
							const float zOverlap = glm::min(playerMaxCorner.z, blockPositionMax.z) - glm::max(playerMinCorner.z, blockPositionMin.z);

							constexpr float normal = 1.0f;
							if (xOverlap < glm::min(yOverlap, zOverlap))
								collisionNormal.x += (playerMaxCorner.x - blockPositionMin.x < blockPositionMax.x - playerMinCorner.x) ? normal : -normal;
							else if (zOverlap < glm::min(xOverlap, yOverlap))
								collisionNormal.z += (playerMaxCorner.z - blockPositionMin.z < blockPositionMax.z - playerMinCorner.z) ? normal : -normal;
							else
							{
								if (playerMaxCorner.y - blockPositionMin.y < blockPositionMax.y - playerMinCorner.y)
								{
									//TODO: Change state
									collisionNormal.y += normal;
								}
								else
								{
									//TODO: Change state
									collisionNormal.y -= normal;
								}
							}
						}
					}
				}
			}
		}
		if (colided)
			collisionNormal = glm::normalize(collisionNormal);
		
		return colided;
	}

}
