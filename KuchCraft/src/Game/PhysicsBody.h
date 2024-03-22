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
		void SetFlyingStatus(bool status);
		void SetCollidingStatus(bool status);

		const glm::vec3& GetPosition() const { return m_Position; }

		void OnUpdate(float dt);

		void MoveForward();
		void SprintForward();
		void MoveBackward();

		void MoveLeft();
		void MoveRight();

		void Jump();
		void FlyUp();
		void FlyDown();
		void SwimUp();

	private:
		void PerformCollsionCheck();
		bool CheckForCollisions(const glm::vec3 position, glm::vec3& out_CollisionVector);
		bool IsOnGround();
		bool IsInWater();

	private:
		glm::vec3 m_FrontDirection { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_RightDirection { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_UpDirection    { 0.0f, 1.0f, 0.0f };

		glm::ivec3 m_CollisionCheckRadius{ 0 };
		AABB       m_PlayerAbsoluteAABB;

		glm::vec3 m_Position       { 0.0f };
		glm::vec3 m_MovementVector { 0.0f };
		glm::vec3 m_JumpVector     { 0.0f };

		float m_HorizontalSpeed = 0.0f;
		float m_VerticalSpeed   = 0.0f;

		bool m_Flying     = false;
		bool m_Colliding  = true;

		bool  m_Sprint          = false;
		bool  m_IsOnGround      = false;
		bool  m_IsInWater       = false;
	};

}