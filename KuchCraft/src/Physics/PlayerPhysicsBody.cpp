#include "kcpch.h"
#include "Physics/PlayerPhysicsBody.h"

#include "World/Block.h"
#include "World/World.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtc/integer.hpp>

namespace KuchCraft {

	constexpr float player_speed        = 5.0f;
	constexpr float player_sprint_speed = 9.0f;
	constexpr float player_jump_speed   = 5.5f;
	constexpr float player_swim_speed   = 2.5f;

	constexpr float player_flying_speed        = 20.0f;
	constexpr float player_flying_sprint_speed = 100.0f;

	constexpr int player_absolute_height = (int)player_height + 1;
	constexpr int player_absolute_width  = (int)player_width  + 1;

	PlayerPhysicsBody::PlayerPhysicsBody()
	{

	}

	PlayerPhysicsBody::~PlayerPhysicsBody()
	{
	}

	void PlayerPhysicsBody::Init(float width, float height)
	{
		const float halfWidth    = width / 2.0f;
		m_PlayerAbsoluteAABB.Min = { -halfWidth, 0.0f,   -halfWidth };
		m_PlayerAbsoluteAABB.Max = { halfWidth, height,  halfWidth };

		m_CollisionCheckRadius.x = static_cast<int>(glm::ceil(width));
		m_CollisionCheckRadius.y = static_cast<int>(glm::ceil(height));
		m_CollisionCheckRadius.z = static_cast<int>(glm::ceil(width));
	}

	void PlayerPhysicsBody::SetData(const glm::vec3& position, const glm::vec3& frontDirection, const glm::vec3& rightDirection)
	{
		m_Position       = position;
		m_FrontDirection = frontDirection;
		m_RightDirection = rightDirection;
	}

	void PlayerPhysicsBody::SetFlyingStatus(bool status)
	{
		m_JumpVector     = { 0.0f, 0.0f, 0.0f };
		m_MovementVector = { 0.0f, 0.0f ,0.0f };
		m_Flying         = status;
		m_VerticalSpeed  = 0.0f;
	}

	void PlayerPhysicsBody::SetCollidingStatus(bool status)
	{
		m_Colliding = status;
	}

	void PlayerPhysicsBody::OnUpdate(float dt)
	{
		if (glm::length(m_MovementVector) > 0.0f)
		{
			m_MovementVector = glm::normalize(m_MovementVector);

			if (m_Flying)
				m_HorizontalSpeed = m_Sprint ? player_flying_sprint_speed : player_flying_speed;
			else
				m_HorizontalSpeed = m_Sprint ? player_sprint_speed        : player_speed;
		}
		
		if (m_IsOnGround)
		{
			m_VerticalSpeed = 0.0f;	
			m_Position     += m_MovementVector * m_HorizontalSpeed * dt;
			m_JumpVector    = m_MovementVector * (m_Sprint ? 0.8f : 0.3f);
		}
		else
		{
			if (!m_Flying)
			{
				constexpr float gravity = 14.0f;

				m_MovementVector.y = -1.0f;
				m_VerticalSpeed   -= m_IsInWater ? gravity / 5.0f * dt : gravity * dt;
			}

			constexpr float vertical_max_speed = 30.0f;
			glm::clamp(m_VerticalSpeed, -vertical_max_speed, vertical_max_speed);

			float speedMultiplier = m_Sprint ? 0.5f : 0.6f;
			const glm::vec3 speed = {  m_HorizontalSpeed * speedMultiplier,
									  -m_VerticalSpeed,
									   m_HorizontalSpeed * speedMultiplier };

			m_Position += (m_MovementVector + m_JumpVector) * speed * dt;
		}

		if (m_Colliding)
		{
			PerformCollsionCheck();
			m_IsOnGround = IsOnGround();
			m_IsInWater  = IsInWater();
		}

		if (m_IsOnGround || m_Flying || !m_IsInWater)
			m_MovementVector  = { 0.0f, 0.0f, 0.0f };	
	}

	void PlayerPhysicsBody::MoveForward()
	{
		m_MovementVector += m_FrontDirection;
		m_Sprint          = false;
	}

	void PlayerPhysicsBody::SprintForward()
	{
		m_MovementVector += m_FrontDirection;
		m_Sprint          = true;	
	}

	void PlayerPhysicsBody::MoveBackward()
	{
		m_MovementVector -= m_FrontDirection;
		m_Sprint          = false;
	}

	void PlayerPhysicsBody::MoveLeft()
	{
		m_MovementVector -= m_RightDirection;
		m_Sprint          = false;
	}

	void PlayerPhysicsBody::MoveRight()
	{
		m_MovementVector += m_RightDirection;
		m_Sprint          = false;
	}

	void PlayerPhysicsBody::Jump()
	{
		if (m_IsOnGround && !m_IsInWater)
		{
			m_MovementVector += m_UpDirection;
			m_IsOnGround      = false;
			m_VerticalSpeed   = player_jump_speed;
		}
	}

	void PlayerPhysicsBody::FlyUp()
	{
		m_MovementVector += m_UpDirection;
		m_VerticalSpeed   = -player_flying_speed;
		m_IsOnGround      = false;
	}

	void PlayerPhysicsBody::FlyDown()
	{
		m_MovementVector -=  m_UpDirection;
		m_VerticalSpeed   = -player_flying_speed;
	}

	void PlayerPhysicsBody::SwimUp()
	{
		if (!m_Flying && m_IsInWater)
		{
			m_MovementVector += m_UpDirection;
			m_IsOnGround      = false;
			m_VerticalSpeed   = player_swim_speed;
		}
	}

	void PlayerPhysicsBody::PerformCollsionCheck()
	{
		glm::vec3 collisionVector{ 0.0f };

		if (CheckForCollisions(m_Position, collisionVector))
		{
			constexpr uint32_t max_correction_attempts = 1000;
			constexpr float    correction_epsilon      = 0.001f;

			glm::vec3 correctedPosition = m_Position;
			uint32_t  attempts          = 0;

			do
			{
				correctedPosition -= collisionVector * correction_epsilon;
				attempts++;
			} 
			while (CheckForCollisions(correctedPosition, collisionVector) && attempts < max_correction_attempts);

			m_Position = correctedPosition;
		}
	}

	bool PlayerPhysicsBody::CheckForCollisions(const glm::vec3& position, glm::vec3& out_CollisionVector)
	{
		bool colided = false;
		out_CollisionVector = { 0.0f, 0.0f, 0.0f };

		const glm::ivec3 absolutePosition{ position };
		AABB playerAABB = m_PlayerAbsoluteAABB.MoveTo(position);

		// Find blocks near player
		for (int x = absolutePosition.x - m_CollisionCheckRadius.x; x <= absolutePosition.x + m_CollisionCheckRadius.x; x++)
		{
			for (int y = absolutePosition.y - m_CollisionCheckRadius.y; y <= absolutePosition.y + m_CollisionCheckRadius.y; y++)
			{
				for (int z = absolutePosition.z - m_CollisionCheckRadius.z; z <= absolutePosition.z + m_CollisionCheckRadius.z; z++)
				{
					constexpr float block_size = 1.0f;
					AABB blockAABB = { glm::vec3{ x, y, z },
						glm::vec3{ x + block_size, y + block_size, z + block_size} };

					const glm::vec3& blockPosition = blockAABB.Min;
					const Block block = World::Get().GetBlock(blockPosition);

					if (!block.IsSolid())
						continue;

					// Check intersection status
					if (!playerAABB.IsColliding(blockAABB))
						continue;

					colided = true;

					// Find overlaping plane
					const glm::vec3 overlaping   = playerAABB.GetOverlaping(blockAABB);
					constexpr float plane_normal = 1.0f;

					if (overlaping.x < glm::min(overlaping.y, overlaping.z))
						out_CollisionVector.x = (playerAABB.Max.x - blockAABB.Min.x < blockAABB.Max.x - playerAABB.Min.x) ? plane_normal : -plane_normal;
					
					else if (overlaping.z < glm::min(overlaping.x, overlaping.y))
						out_CollisionVector.z = (playerAABB.Max.z - blockAABB.Min.z < blockAABB.Max.z - playerAABB.Min.z) ? plane_normal : -plane_normal;
					
					else
					{
						if (playerAABB.Max.y - blockAABB.Min.y < blockAABB.Max.y - playerAABB.Min.y)
						{
							out_CollisionVector.y =  plane_normal;
							m_VerticalSpeed       =  0.0f;
						}
						else
							out_CollisionVector.y = -plane_normal;
					}
				}
			}
		}

		if (colided)
		{
			out_CollisionVector = glm::normalize(out_CollisionVector);
			m_JumpVector        = { 0.0f, 0.0f, 0.0f };
		}

		return colided;
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

				const glm::vec3 overlaping = playerAABB.GetOverlaping(blockAABB);
				if (overlaping.y < glm::min(overlaping.x, overlaping.z))
				{
					if (playerAABB.Max.y - blockAABB.Min.y >= blockAABB.Max.y - playerAABB.Min.y)
						return true;
				}
			}
		}
		return false;
	}
	bool PlayerPhysicsBody::IsInWater()
	{
		return World::Get().GetBlock(m_Position) == BlockType::Water;
	}
}