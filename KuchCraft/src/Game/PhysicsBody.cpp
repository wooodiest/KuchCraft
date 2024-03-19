#include "PhysicsBody.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtc/integer.hpp>

#include "Core/Log.h"
#include "World/Block.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "Core/Utils.h"

namespace KuchCraft {

	constexpr float gravity = 9.8f;

	constexpr float vertical_max_speed   = 30.0f;

	PlayerPhysicsBody::PlayerPhysicsBody(float width, float height)
	{
		const float halfWidth = width / 2.0f;
		m_PlayerAbsoluteAABB.Min = { -halfWidth, 0.0f,   -halfWidth };
		m_PlayerAbsoluteAABB.Max = {  halfWidth, height,  halfWidth };

		m_CollisionCheckRadius.x = static_cast<int>(glm::ceil(width));
		m_CollisionCheckRadius.y = static_cast<int>(glm::ceil(height));
		m_CollisionCheckRadius.z = static_cast<int>(glm::ceil(width));
	}

	PlayerPhysicsBody::~PlayerPhysicsBody()
	{
	}

	void PlayerPhysicsBody::SetData(const glm::vec3& position, const glm::vec3& frontDirection, const glm::vec3& rightDirection)
	{
		m_Position       = position;
		m_FrontDirection = frontDirection;
		m_RightDirection = rightDirection;
	}

	void PlayerPhysicsBody::OnUpdate(float dt)
	{
		if (glm::length(m_MovementVector) > 0.0f)
		{
			m_MovementVector = glm::normalize(m_MovementVector);
			if (m_Flying)
				m_HorizontalSpeed = m_Sprint ? player_sprint_flying_speed : player_flying_speed;
			else
				m_HorizontalSpeed = m_Sprint ? player_sprint_speed : player_speed;
		}
		
		if (m_IsOnGround)
		{
			m_VerticalSpeed = 0.0f;	
			m_Position += m_MovementVector * m_HorizontalSpeed * dt;
		}
		else
		{
			if (!m_Flying)
			{
				m_MovementVector.y = -1.0f;
				m_VerticalSpeed -= gravity * dt;
			}

			glm::clamp(m_VerticalSpeed, -vertical_max_speed, vertical_max_speed);

			constexpr float not_on_ground_horizontal_muliplier = 0.6f;
			const glm::vec3 speed = {  m_HorizontalSpeed * not_on_ground_horizontal_muliplier,
									  -m_VerticalSpeed,
									   m_HorizontalSpeed * not_on_ground_horizontal_muliplier };

			m_Position += m_MovementVector * speed * dt;
		}

		constexpr bool checkForCollision = true; // temporarily;
		if (checkForCollision)
			PerformCollsionCheck();

		m_IsOnGround = IsOnGround();
		ResetMovementVector();
	}

	void PlayerPhysicsBody::MoveForward()
	{
		m_MovementVector += m_FrontDirection;
	}

	void PlayerPhysicsBody::SprintForward()
	{
		m_MovementVector += m_FrontDirection;
		m_Sprint = true;
	}

	void PlayerPhysicsBody::MoveBackward()
	{
		m_MovementVector -= m_FrontDirection;
		m_Sprint = false;
	}

	void PlayerPhysicsBody::MoveLeft()
	{
		m_MovementVector -= m_RightDirection;
		m_Sprint = false;
	}

	void PlayerPhysicsBody::MoveRight()
	{
		m_MovementVector +=  m_RightDirection;
		m_Sprint = false;
	}

	void PlayerPhysicsBody::Jump()
	{
		if (m_IsOnGround)
		{
			m_IsOnGround = false;
			m_VerticalSpeed = player_jump_speed;
			m_MovementVector += m_UpDirection;
		}
	}

	void PlayerPhysicsBody::FlyUp()
	{
		if (m_IsOnGround)
			m_IsOnGround = false;
		m_MovementVector += m_UpDirection;

		m_VerticalSpeed = -player_flying_speed;
	}

	void PlayerPhysicsBody::FlyDown()
	{
		m_MovementVector -= m_UpDirection;
		m_VerticalSpeed = -player_flying_speed;
	}

	void PlayerPhysicsBody::PerformCollsionCheck()
	{
		glm::vec3 collisionVector{ 0.0f };

		if (CheckForCollisions(m_Position, collisionVector))
		{
			constexpr uint32_t max_correction_attempts = 1000;
			constexpr float correction_epsilon = 0.001f;

			glm::vec3 correctedPosition = m_Position - collisionVector * correction_epsilon;
			if (CheckForCollisions(correctedPosition, collisionVector))
			{
				uint32_t attempts = 0;
				while (CheckForCollisions(correctedPosition, collisionVector) && attempts < max_correction_attempts)
				{
					correctedPosition -= collisionVector * correction_epsilon;
					attempts++;
				}
				m_Position = correctedPosition;;
			}
		}
	}

	bool PlayerPhysicsBody::CheckForCollisions(const glm::vec3 position, glm::vec3& out_CollisionVector)
	{
		bool colided = false;
		out_CollisionVector = { 0.0f, 0.0f, 0.0f };

		const glm::ivec3 absolutePosition{ position };
		AABB playerAABB = m_PlayerAbsoluteAABB.MoveTo(position);

		// Find blocks near player
		for (int x = -m_CollisionCheckRadius.x; x <= m_CollisionCheckRadius.x; x++)
		{
			for (int y = -m_CollisionCheckRadius.y; y <= m_CollisionCheckRadius.y; y++)
			{
				for (int z = -m_CollisionCheckRadius.z; z <= m_CollisionCheckRadius.z; z++)
				{
					constexpr float block_size = 1.0f;
					AABB blockAABB = { glm::vec3{ absolutePosition.x + x, absolutePosition.y + y, absolutePosition.z + z },
						glm::vec3{ absolutePosition.x + x + block_size, absolutePosition.y + y + block_size, absolutePosition.z + z + block_size} };

					const glm::vec3& blockPosition = blockAABB.Min;
					const Block block = World::Get().GetBlock(blockPosition);

					if (!block.IsSolid())
						continue;

					// Check intersection status
					if (!playerAABB.IsColliding(blockAABB))
						continue;

					colided = true;

					const float xOverlap = glm::min(playerAABB.Max.x, blockAABB.Max.x) - glm::max(playerAABB.Min.x, blockAABB.Min.x);
					const float yOverlap = glm::min(playerAABB.Max.y, blockAABB.Max.y) - glm::max(playerAABB.Min.y, blockAABB.Min.y);
					const float zOverlap = glm::min(playerAABB.Max.z, blockAABB.Max.z) - glm::max(playerAABB.Min.z, blockAABB.Min.z);

					if (xOverlap < glm::min(yOverlap, zOverlap))
					{
						out_CollisionVector.x = (playerAABB.Max.x - blockAABB.Min.x < blockAABB.Max.x - playerAABB.Min.x) ? 1.0f : -1.0f;
					}
					else if (zOverlap < glm::min(xOverlap, yOverlap))
					{
						out_CollisionVector.z = (playerAABB.Max.z - blockAABB.Min.z < blockAABB.Max.z - playerAABB.Min.z) ? 1.0f : -1.0f;
					}
					else
					{
						// TODO: Add more stuff to y collision detection
						if (playerAABB.Max.y - blockAABB.Min.y < blockAABB.Max.y - playerAABB.Min.y)
						{
							out_CollisionVector.y = 1.0f;
							m_VerticalSpeed = 0.0f;
						}
						else
						{
							out_CollisionVector.y = -1.0f;
						}
					}
				}
			}
		}

		if (colided)
			out_CollisionVector = glm::normalize(out_CollisionVector);

		return colided;
	}

	void PlayerPhysicsBody::ResetMovementVector()
	{
		m_MovementVector = { 0.0f, 0.0f, 0.0f };
		m_HorizontalSpeed = 0.0f;
		m_Sprint = false;
	}

	bool PlayerPhysicsBody::IsOnGround()
	{
		constexpr float down_position_diff = 0.001f;
		const glm::vec3 downPosition = m_Position + glm::vec3(0.0f, -down_position_diff, 0.0f);

		const glm::ivec3 absolutePosition{ downPosition };
		AABB playerAABB = m_PlayerAbsoluteAABB.MoveTo(downPosition);

		for (int x = -m_CollisionCheckRadius.x; x <= m_CollisionCheckRadius.x; x++)
		{
			for (int z = -m_CollisionCheckRadius.z; z <= m_CollisionCheckRadius.z; z++)
			{
				constexpr float block_size = 1.0f;
				AABB blockAABB = { glm::vec3{ absolutePosition.x + x, absolutePosition.y, absolutePosition.z + z },
					glm::vec3{ absolutePosition.x + x + block_size, absolutePosition.y + block_size, absolutePosition.z + z + block_size} };

				const glm::vec3& blockPosition = blockAABB.Min;
				const Block block = World::Get().GetBlock(blockPosition);

				if (!block.IsSolid())
					continue;

				if (!playerAABB.IsColliding(blockAABB))
					continue;

				const float xOverlap = glm::min(playerAABB.Max.x, blockAABB.Max.x) - glm::max(playerAABB.Min.x, blockAABB.Min.x);
				const float yOverlap = glm::min(playerAABB.Max.y, blockAABB.Max.y) - glm::max(playerAABB.Min.y, blockAABB.Min.y);
				const float zOverlap = glm::min(playerAABB.Max.z, blockAABB.Max.z) - glm::max(playerAABB.Min.z, blockAABB.Min.z);

				if (yOverlap < glm::min(xOverlap, zOverlap))
				{
					if (playerAABB.Max.y - blockAABB.Min.y >= blockAABB.Max.y - playerAABB.Min.y)
					{
						return true;
					}
				}
			}
		}
		return false;
	}
}