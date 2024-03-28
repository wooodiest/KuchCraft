#include "kcpch.h"
#include "Game/Player.h"

#include "World/World.h"

#include "Renderer/Renderer.h"

#include "Core/Random.h"
#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Random.h"
#include "Core/Utils.h"

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
		KC_PROFILE_FUNCTION();

		m_PrevMousePosition = Input::GetMousePosition();

		m_PhysicsBody.Init(player_width, player_height);
		SetGameMode(m_GameMode);
	}


	Player::~Player()
	{
	}

	void Player::OnUpdate(float dt)
	{
		KC_PROFILE_FUNCTION();

		// Movement	
		// Mouse
		{
			glm::vec2 position     = Input::GetMousePosition();
			glm::vec2 positionDiff = position - m_PrevMousePosition;
			m_PrevMousePosition    = position;

			constexpr float camera_sensitivity = 0.25f;
			m_Rotation.x += positionDiff.x * camera_sensitivity * 0.001f;
			m_Rotation.y -= positionDiff.y * camera_sensitivity * 0.001f;

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

		if (m_GameMode != GameMode::Spectator)
			m_TargetedBlock = GetTargetBlockInfo();
		else
			m_TargetedBlock.Targeted = false;
	}

	void Player::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(KC_BIND_EVENT_FN(Player::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(KC_BIND_EVENT_FN(Player::OnMouseButtonPressed));

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

		if (m_TargetedBlock.Targeted)
		{
			m_DebugText += "\n   Targeted block: " + World::Get().GetBlock(m_TargetedBlock.Position).GetName();
			switch (m_TargetedBlock.Plane)
			{
				case PlaneDirection::Left:   m_DebugText += " - left - ";   break;
				case PlaneDirection::Right:  m_DebugText += " - right - ";  break;
				case PlaneDirection::Bottom: m_DebugText += " - bottom - "; break;
				case PlaneDirection::Top:    m_DebugText += " - top - ";    break;
				case PlaneDirection::Behind: m_DebugText += " - behind - "; break;
				case PlaneDirection::Front:  m_DebugText += " - front - ";  break;
			}
			m_DebugText += VecToString(m_TargetedBlock.Position);
		}
		else
			m_DebugText += "\n   Targeted block: none";

		return m_DebugText;
	}

	TargetedBlockInfo Player::GetTargetBlockInfo()
	{
		KC_PROFILE_FUNCTION();

		TargetedBlockInfo outputTargetedBlockInfo;

		const glm::ivec3 absolutePosition{ m_Position };
		constexpr float range          = 3.5f;
		constexpr int   absolute_range = 4;

		float currentDistance = std::numeric_limits<float>::infinity();
		const Ray ray = { GetEyePosition(), m_Camera.GetFront(), range };

		// Find blocks near player
		for (int x = absolutePosition.x - absolute_range; x <= absolutePosition.x + absolute_range; x++)
		{
			for (int y = absolutePosition.y - absolute_range; y <= absolutePosition.y + absolute_range; y++)
			{
				for (int z = absolutePosition.z - absolute_range; z <= absolutePosition.z + absolute_range; z++)
				{
					constexpr float block_size = 1.0f;
					AABB blockAABB = { glm::vec3{ x, y, z },
						glm::vec3{x + block_size, y + block_size, z + block_size} };

					const glm::vec3& blockPosition = blockAABB.Min;
					const Block block = World::Get().GetBlock(blockPosition);

					if (!block.IsSolid())
						continue;

					auto targetedBlockInfo = ray.IsColliding(blockAABB);
					if (targetedBlockInfo.Targeted)
					{
						glm::vec3 blockCenter = blockPosition + glm::vec3(block_size / 2.0f);
						float distance = glm::length2(ray.Origin - blockCenter);

						if (distance < currentDistance)
						{
							outputTargetedBlockInfo = targetedBlockInfo;
							currentDistance         = distance;	
						}
					}
				}
			}
		}
		return outputTargetedBlockInfo;
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

	bool Player::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
			case MouseCode::ButtonLeft:
			{
				if (m_GameMode != GameMode::Spectator)
					DestroyBlock(m_TargetedBlock.Position);
				return false;
			}
			case MouseCode::ButtonRight:
			{
				if (m_GameMode != GameMode::Spectator)
					PlaceBlock(m_TargetedBlock.Position, Block(BlockType(Random::UInt(2, total_number_of_block_types))));
				return false;
			}
		}
		
		return false;
	}

	void Player::PlaceBlock(const glm::vec3& position, const Block& block)
	{
		KC_PROFILE_FUNCTION();

		if (!m_TargetedBlock.Targeted)
			return;

		glm::vec3 newPosition = position;
		switch (m_TargetedBlock.Plane)
		{
			case PlaneDirection::Left:   newPosition.x -= 1.0f; break;
			case PlaneDirection::Right:  newPosition.x += 1.0f; break;
			case PlaneDirection::Bottom: newPosition.y -= 1.0f; break;
			case PlaneDirection::Top:    newPosition.y += 1.0f; break;
			case PlaneDirection::Behind: newPosition.z -= 1.0f; break;
			case PlaneDirection::Front:  newPosition.z += 1.0f; break;
		}

		const glm::ivec3 blockAbsolutePosition{ newPosition };
		constexpr uint32_t block_size = 1;
		const AABB blockAABB  = { blockAbsolutePosition, blockAbsolutePosition + glm::ivec3(block_size)};
		const AABB playerAABB = m_PhysicsBody.GetPlayerAbsoluteAABB().MoveTo(m_Position);

		if (playerAABB.IsColliding(blockAABB))
			return;
		else
			World::Get().SetBlock(newPosition, block);
	}

	void Player::DestroyBlock(const glm::vec3& position)
	{
		KC_PROFILE_FUNCTION();

		if (!m_TargetedBlock.Targeted)
			return;

		World::Get().SetBlock(position, Block(BlockType::Air));
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

}
