#include "Player.h"

#include "Core/Random.h"
#include "World/World.h"

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Utils.h"

#include "Renderer/Renderer.h"

#include <glm/gtc/epsilon.hpp>
#include <iostream>

namespace KuchCraft {

	Player::Player()
	{
		Init();
	}

	Player::Player(const glm::vec3& position, const glm::vec2& rotation, GameMode gameMode)
		: m_Position(position), m_Rotation(rotation), m_GameMode(gameMode)
	{
		Init();
	}

	void Player::Init()
	{
		SetRenderDistance(m_GraphicalSettings.RenderDistance, false);
		m_PrevMousePosition = Input::GetMousePosition();

		m_PhysicsBody.Init(player_width, player_height);
		SetGameMode(m_GameMode);
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

		if (Input::IsKeyPressed(KeyCode::Space))
			m_PhysicsBody.SwimUp();

		if ((m_GameMode == GameMode::Creative || m_GameMode == GameMode::Spectator) && m_PhysicsBody.GetFlyingStatus())
		{
			if (Input::IsKeyPressed(KeyCode::Space))
				m_PhysicsBody.FlyUp();
			if (Input::IsKeyPressed(KeyCode::LeftControl))
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

	std::string& Player::GetDebugText()
	{
		std::string gameMode;
		switch (m_GameMode)
		{
			case KuchCraft::GameMode::Survival:  gameMode = "Survival"; break;
			case KuchCraft::GameMode::Creative:  gameMode = "Creative"; break;
			case KuchCraft::GameMode::Spectator: gameMode = "Spectator"; break;
		}
		m_DebugText =
			  "\nPlayer:"
			  "\n   Position: "  + VecToString(m_Position) 
			+ "\n   Rotation: "  + VecToString(glm::vec2(glm::degrees(m_Rotation.x), glm::degrees(m_Rotation.y)))
			+ "\n   Gamemode: "  + gameMode;

		return m_DebugText;
	}

	bool Player::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		switch (e.GetKeyCode())
		{
			case KeyCode::Space:
			{
				m_PhysicsBody.Jump();
				return false;
			}
			case KeyCode::CapsLock:
			{
				if (m_GameMode == GameMode::Creative)
					m_PhysicsBody.SetFlyingStatus(!m_PhysicsBody.GetFlyingStatus());
				
				return false;
			}	
			case KeyCode::F4:
			{
				SetGameMode(GameMode::Survival);
				return false;
			}
			case KeyCode::F5:
			{
				SetGameMode(GameMode::Creative);
				return false;
			}
			case KeyCode::F6:
			{
				SetGameMode(GameMode::Spectator);
				return false;
			}
		}
		return false;
	}

	void Player::SetGameMode(GameMode gameMode)
	{
		m_GameMode = gameMode;
		switch (m_GameMode)
		{
			case KuchCraft::GameMode::Survival:
			{
				m_PhysicsBody.SetFlyingStatus(false);
				m_PhysicsBody.SetCollidingStatus(true);
				break;
			}
			case KuchCraft::GameMode::Creative:
			{
				m_PhysicsBody.SetCollidingStatus(true);
				break;
			}
			case KuchCraft::GameMode::Spectator:
			{
				m_PhysicsBody.SetFlyingStatus(true);
				m_PhysicsBody.SetCollidingStatus(false);
				break;
			}
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
