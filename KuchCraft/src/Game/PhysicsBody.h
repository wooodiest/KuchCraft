#pragma once

#include <glm/glm.hpp>
#include <string>
#include "Physics/AABB.h"

namespace KuchCraft {

	class PlayerPhysicsBody
	{
	public:
		PlayerPhysicsBody(float width, float height);
		~PlayerPhysicsBody();

		void SetData(const glm::vec3& position, const glm::vec3& frontDirection, const glm::vec3& rightDirection);
		glm::vec3 GetNewPosition() { return m_NewPosition; }

		void OnUpdate(float dt);

		void MoveForward();
		void SprintForward();
		void MoveBackward();

		void MoveLeft();
		void MoveRight();

		void Jump();
		void FlyUp();
		void FlyDown();

	private:
		bool CheckForCollisions(const glm::vec3 position, glm::vec3& out_CollisionVector);
		void ResetMovementVector();
		bool IsOnGround();
	private:
		glm::vec3 m_FrontDirection{ 0.0f };
		glm::vec3 m_RightDirection{ 0.0f };
		glm::vec3 m_UpDirection{ 0.0f, 1.0f, 0.0f };

		glm::vec3 m_Position{ 0.0f };
		glm::vec3 m_NewPosition{ 0.0f };

		glm::ivec3 m_CollisionCheckRadius{ 0 };
		AABB m_PlayerAbsoluteAABB;

		glm::vec3 m_MovementVector{ 0.0f };
		bool m_IsGrounded = false;
	};

}