#include "PhysicsBody.h"

#include <glm/gtc/integer.hpp>
#include "Core/Log.h"
#include "World/Block.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "Core/Utils.h"

namespace KuchCraft {

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
		constexpr float gravity = 9.8f;

		m_IsGrounded = IsOnGround();

		if (!m_IsGrounded) 
			m_MovementVector.y = -1.0f;
		
		if (glm::length(m_MovementVector) > 0.0f)
			m_MovementVector = glm::normalize(m_MovementVector);

		if (!m_IsGrounded)
		{
			const float movement_speed = 7.0f;
			const glm::vec3 speed = { movement_speed, gravity, movement_speed };
			m_NewPosition = m_Position + m_MovementVector * speed * dt;
		}
		else
		{
			const float movement_speed = 7.0f;
			const glm::vec3 horizontal_speed = { m_MovementVector.x * movement_speed, m_MovementVector.y * movement_speed, m_MovementVector.z * movement_speed };
			m_NewPosition = m_Position + horizontal_speed * dt;
		}

		constexpr bool checkForCollision = true; // temporarily;
		if (checkForCollision)
		{
			glm::vec3 collisionVector{ 0.0f };

			if (CheckForCollisions(m_NewPosition, collisionVector))
			{
				constexpr uint32_t max_correction_attempts = 1000;
				constexpr float correction_epsilon = 0.001f;

				glm::vec3 correctedPosition = m_NewPosition;
				if (CheckForCollisions(m_NewPosition, collisionVector))
				{
					uint32_t attempts = 0;
					while (CheckForCollisions(correctedPosition, collisionVector) && attempts < max_correction_attempts)
					{
						correctedPosition -= collisionVector * correction_epsilon;
						attempts++;
					}
					m_NewPosition = correctedPosition;;
				}
			}
		}

		ResetMovementVector();
	}

	void PlayerPhysicsBody::MoveForward()
	{
		m_MovementVector += m_FrontDirection;
	}

	void PlayerPhysicsBody::SprintForward()
	{
	}

	void PlayerPhysicsBody::MoveBackward()
	{
		m_MovementVector -= m_FrontDirection;
	}

	void PlayerPhysicsBody::MoveLeft()
	{
		m_MovementVector -= m_RightDirection;
	}

	void PlayerPhysicsBody::MoveRight()
	{
		m_MovementVector += m_RightDirection;
	}

	void PlayerPhysicsBody::Jump()
	{
	}

	void PlayerPhysicsBody::FlyUp()
	{
		m_MovementVector += m_UpDirection;
		m_IsGrounded = false;
	}

	void PlayerPhysicsBody::FlyDown()
	{
		m_MovementVector -= m_UpDirection;
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
	}

	bool PlayerPhysicsBody::IsOnGround()
	{
		constexpr float down_position_diff = 0.001f;
		const glm::vec3 downPosition = m_Position + glm::vec3(0.0f, -down_position_diff, 0.0f);

		const glm::ivec3 absolutePosition{ downPosition };
		AABB playerAABB = m_PlayerAbsoluteAABB.MoveTo(downPosition);

		// Find blocks near player
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