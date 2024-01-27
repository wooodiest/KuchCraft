#include "Player.h"

#include "Core/Input.h"
#include "Core/Random.h"
#include "World/World.h"

namespace KuchCraft {

	Player::Player()
	{
	}

	Player::~Player()
	{
	}

	void Player::OnUpdate(float dt)
	{
		// Movement
		bool sprint = false;
		if (Input::IsKeyPressed(KeyCode::LeftShift))
			sprint = true;

		m_Camera.SetData({m_Position.x, m_Position.y + m_PlayerHeight, m_Position.z}, m_Rotation, m_MovementSpeed, m_MovementSprintSpeed,  dt);

		if (Input::IsKeyPressed(KeyCode::W))
			m_Camera.OnKeyboardMovement(KeyboardMovement::Forward, sprint);
		if (Input::IsKeyPressed(KeyCode::S))
			m_Camera.OnKeyboardMovement(KeyboardMovement::Backward);

		if (Input::IsKeyPressed(KeyCode::A))
			m_Camera.OnKeyboardMovement(KeyboardMovement::Left);
		if (Input::IsKeyPressed(KeyCode::D))
			m_Camera.OnKeyboardMovement(KeyboardMovement::Right);

		if (Input::IsKeyPressed(KeyCode::Space))
			m_Camera.OnKeyboardMovement(KeyboardMovement::Up);
		if (Input::IsKeyPressed(KeyCode::LeftControl))
			m_Camera.OnKeyboardMovement(KeyboardMovement::Down);
		
		m_Camera.OnMouseMovement();
		m_Camera.OnUpdate();

		auto& pos = m_Camera.GetPosition();
		m_Position = { pos.x, pos.y - m_PlayerHeight, pos.z };
		m_Rotation = m_Camera.GetRotation();	

		// Test
		if (Input::IsMouseButtonPressed(MouseCode::ButtonLeft))
		{
			glm::vec3 position = { m_Position.x + 2.0f, m_Position.y, m_Position.z + 2.0f };
			Block block;
			block.blockType = static_cast<BlockType>(Random::Int(1, (int)BlockType::LastElement - 1));
			World::SetBlock(position, block);
		}

	}

}