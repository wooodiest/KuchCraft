#include "Player.h"

#include "Core/Random.h"
#include "World/World.h"

#include <iostream>

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

		m_Camera.SetData({ m_Position.x, m_Position.y + m_PlayerMovementSettings.Height, m_Position.z },
						   m_Rotation,
						   m_PlayerMovementSettings.Speed, m_PlayerMovementSettings.SprintSpeed, 
						   dt);

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

		auto& pos  = m_Camera.GetPosition();
		m_Position = { pos.x, pos.y - m_PlayerMovementSettings.Height, pos.z };
		m_Rotation = m_Camera.GetRotation();	

	}

}