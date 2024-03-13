#include "Player.h"

#include "Core/Random.h"
#include "World/World.h"

#include "Core/Core.h"

#include <iostream>

namespace KuchCraft {

	Player::Player()
	{
		OnRenderDistanceChanged(m_GraphicalSettings.RenderDistance);
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

}
