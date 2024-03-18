#include "PhysicsBody.h"

#include <glm/gtc/integer.hpp>
#include "Core/Log.h"
#include "World/Block.h"
#include "World/World.h"

namespace KuchCraft {

	PlayerPhysicsBody::PlayerPhysicsBody(float width, float height)
	{
		const float halfWidth = width / 2.0f;
		m_ColideBox.Min = { -halfWidth, 0.0f,   -halfWidth };
		m_ColideBox.Max = {  halfWidth, height,  halfWidth };

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
			m_MovementVector = glm::normalize(m_MovementVector);

		constexpr float movement_speed = 7.0f; // temporarily
		m_NewPosition = m_Position + m_MovementVector * movement_speed * dt;;

		constexpr bool checkForCollision = true; // temporarily
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
					m_NewPosition = correctedPosition;
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
	}

	void PlayerPhysicsBody::FlyDown()
	{
		m_MovementVector -= m_UpDirection;
	}

	bool PlayerPhysicsBody::CheckForCollisions(const glm::vec3 position, glm::vec3& out_CollisionVector)
	{
		bool colided = false;

		const glm::ivec3 absolutePosition{ position };
		const glm::vec3 playerMinCorner = m_ColideBox.Min + position;
		const glm::vec3 playerMaxCorner = m_ColideBox.Max + position;

		// Find blocks near player
		for (int x = -m_CollisionCheckRadius.x; x <= m_CollisionCheckRadius.x; x++)
		{
			for (int y = -m_CollisionCheckRadius.y; y <= m_CollisionCheckRadius.y; y++)
			{
				for (int z = -m_CollisionCheckRadius.z; z <= m_CollisionCheckRadius.z; z++)
				{
					const glm::vec3  blockMinCorner = { absolutePosition.x + x, absolutePosition.y + y, absolutePosition.z + z };
					const glm::vec3& blockPosition  = blockMinCorner;

					const Block block = World::Get().GetBlock(blockPosition);

					if (!block.IsSolid())
						continue;

					constexpr float block_size = 1.0f;
					const glm::vec3 blockMaxCorner{ blockMinCorner.x + block_size, blockMinCorner.y + block_size, blockMinCorner.z + block_size };

					// Check intersection status
					if (playerMinCorner.x >= blockMaxCorner.x || playerMaxCorner.x <= blockMinCorner.x ||
						playerMinCorner.y >= blockMaxCorner.y || playerMaxCorner.y <= blockMinCorner.y ||
						playerMinCorner.z >= blockMaxCorner.z || playerMaxCorner.z <= blockMinCorner.z)
						continue;

					colided = true;

					const float xOverlap = glm::min(playerMaxCorner.x, blockMaxCorner.x) - glm::max(playerMinCorner.x, blockMinCorner.x);
					const float yOverlap = glm::min(playerMaxCorner.y, blockMaxCorner.y) - glm::max(playerMinCorner.y, blockMinCorner.y);
					const float zOverlap = glm::min(playerMaxCorner.z, blockMaxCorner.z) - glm::max(playerMinCorner.z, blockMinCorner.z);

					if (xOverlap < glm::min(yOverlap, zOverlap))
					{
						out_CollisionVector.x += (playerMaxCorner.x - blockMinCorner.x < blockMaxCorner.x - playerMinCorner.x) ? 1.0f : -1.0f;
					}
					else if (zOverlap < glm::min(xOverlap, yOverlap))
					{
						out_CollisionVector.z += (playerMaxCorner.z - blockMinCorner.z < blockMaxCorner.z - playerMinCorner.z) ? 1.0f : -1.0f;
					}
					else
					{
						// TODO: Add more stuff to y collision detection
						if (playerMaxCorner.y - blockMinCorner.y < blockMaxCorner.y - playerMinCorner.y)
						{
							out_CollisionVector.y += 1.0f;
						}
						else
						{
							out_CollisionVector.y += -1.0f;
						}
					}
				}
			}
		}

		if (colided && glm::length(out_CollisionVector))
			out_CollisionVector = glm::normalize(out_CollisionVector);
		
		return colided;
	}

	void PlayerPhysicsBody::ResetMovementVector()
	{
		m_MovementVector = { 0.0f, 0.0f, 0.0f };
	}

}