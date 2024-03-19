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
		m_PhysicsBody.SetData(m_Position, m_Camera.GetAbsoluteFront(), m_Camera.GetAbsoluteRight());

		if (Input::IsKeyPressed(KeyCode::W))
		{
			if (Input::IsKeyPressed(KeyCode::LeftShift))
				m_PhysicsBody.SprintForward();
			else
				m_PhysicsBody.MoveForward();
		}
		if (Input::IsKeyPressed(KeyCode::S))
			m_PhysicsBody.MoveBackward();
		
		if (Input::IsKeyPressed(KeyCode::A))
			m_PhysicsBody.MoveLeft();
		if (Input::IsKeyPressed(KeyCode::D))
			m_PhysicsBody.MoveRight();
		
		if (m_PerformJump)
		{
			m_PhysicsBody.Jump();
			m_PerformJump = false;
		}

		if (Input::IsKeyPressed(KeyCode::Space))
		{
			if (m_FlyingStatus)
				m_PhysicsBody.FlyUp();
		}
		
		if (Input::IsKeyPressed(KeyCode::LeftControl))
		{
			if (m_GameMode == GameMode::Creative || m_GameMode == GameMode::Spectator)
				m_PhysicsBody.FlyDown();
		}
		
		m_PhysicsBody.OnUpdate(dt);

		m_Position = m_PhysicsBody.GetPosition();
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
			case KeyCode::Space: m_PerformJump = true; return false;
			case KeyCode::CapsLock:
				if (m_GameMode == GameMode::Creative || m_GameMode == GameMode::Spectator)
				{
					m_FlyingStatus = !m_FlyingStatus;
					m_PhysicsBody.SetFlyingStatus(m_FlyingStatus);
				}
				return false;
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

}
