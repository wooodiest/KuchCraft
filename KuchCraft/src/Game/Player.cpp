#include "kcpch.h"
#include "Game/Player.h"

#include "World/World.h"

#include "Renderer/Renderer.h"
#include "Renderer/Renderer3D/Renderer3D.h"

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

			constexpr float camera_sensitivity = 0.25f;
			m_Rotation.x += positionDiff.x * camera_sensitivity * 0.001f;
			m_Rotation.y -= positionDiff.y * camera_sensitivity * 0.001f;

			constexpr float min_pitch = glm::radians(-89.9f);
			constexpr float max_pitch = glm::radians( 89.9f);

			if (m_Rotation.y < min_pitch)
				m_Rotation.y = min_pitch;

			if (m_Rotation.y > max_pitch)
				m_Rotation.y = max_pitch;

			constexpr float yaw_boundary = glm::radians(360.0f);

			if (m_Rotation.x > yaw_boundary)
				m_Rotation.x -= yaw_boundary;

			if (m_Rotation.x < 0.0f)
				m_Rotation.x += yaw_boundary;
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
			m_TargetedItem = GetTargetItemInfo();
		else
			m_TargetedItem.Targeted = false;

		if (World::Get().GetItem(GetEyePosition()).Type == ItemType::Water)
			Renderer3D::DrawWaterTinted();

		if (m_TargetedItem.Targeted)
			Renderer3D::DrawOutlinedCube(m_TargetedItem.Position, m_TargetedItem.Size);
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
			case KuchCraft::GameMode::Survival:  gameMode = "Survival";  break;
			case KuchCraft::GameMode::Creative:  gameMode = "Creative";  break;
			case KuchCraft::GameMode::Spectator: gameMode = "Spectator"; break;
		}

		m_DebugText =
			  "\nPlayer:"
			  "\n   Position: "  + VecToString(m_Position, 2)
			+ "\n   Rotation: "  + VecToString(glm::vec2{ glm::degrees(m_Rotation.x), glm::degrees(m_Rotation.y) }, 2)
			+ "\n   Gamemode: "  + gameMode;

		if (m_TargetedItem.Targeted)
			m_DebugText += "\n   Targeted item - " + World::Get().GetItem(m_TargetedItem.Position).GetName() + " : " + VecToString(glm::ivec3{m_TargetedItem.Position});
		else
			m_DebugText += "\n   Targeted item: none";

		return m_DebugText;
	}

	TargetedItemInfo Player::GetTargetItemInfo()
	{
		TargetedItemInfo outputTargetedItemInfo;

		const glm::ivec3 absolutePosition{ m_Position };
		constexpr float  range          = 3.5f;
		constexpr int    absolute_range = 4;

		float currentDistance = std::numeric_limits<float>::infinity();
		const Ray ray = { GetEyePosition(), m_Camera.GetFront(), range };

		// Find blocks near player
		for (int x = absolutePosition.x - absolute_range; x <= absolutePosition.x + absolute_range; x++)
		{
			for (int y = absolutePosition.y - absolute_range; y <= absolutePosition.y + absolute_range; y++)
			{
				for (int z = absolutePosition.z - absolute_range; z <= absolutePosition.z + absolute_range; z++)
				{
					const Item item = World::Get().GetItem({ x, y, z });

					if (!item.IsSolidBlock() && !item.IsFoliageQuad())
						continue;

					AABB itemAABB = item.GetAABB({ x, y, z });

					auto targetedItemInfo = ray.IsColliding(itemAABB);
					if (targetedItemInfo.Targeted)
					{
						targetedItemInfo.Position = itemAABB.GetPosition();
						float distance = glm::length2(ray.Origin - targetedItemInfo.Position);

						if (distance < currentDistance)
						{
							targetedItemInfo.Size   = itemAABB.GetSize();
							outputTargetedItemInfo  = targetedItemInfo;
							currentDistance         = distance;	
						}
					}
				}
			}
		}
		return outputTargetedItemInfo;
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
					DestroyItem(m_TargetedItem.Position);
				return false;
			}
			case MouseCode::ButtonRight:
			{
				if (m_GameMode != GameMode::Spectator)
					PlaceItem(m_TargetedItem.Position, Item(ItemType::RedTulipan));
				return false;
			}
		}
		
		return false;
	}

	void Player::PlaceItem(const glm::vec3& position, Item& block)
	{
		if (!m_TargetedItem.Targeted)
			return;

		glm::vec3 newPosition = position;
		switch (m_TargetedItem.Plane)
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

		if (playerAABB.IsColliding(blockAABB) && !block.IsFoliageQuad())
			return;

		// Calculate block rotation
		float rotation = m_Rotation.x;
		if (      rotation >= glm::radians(90.0f - 45.0f) && rotation < glm::radians(90.0f + 45.0f))	
			block.Rotation = ItemRotation::Deg180;
		else if (rotation >= glm::radians(180.0f - 45.0f) && rotation < glm::radians(180.0f + 45.0f))
			block.Rotation = ItemRotation::Deg270;
		else if (rotation >= glm::radians(270.0f - 45.0f) && rotation < glm::radians(270.0f + 45.0f))
			block.Rotation = ItemRotation::Deg0;
		else
			block.Rotation = ItemRotation::Deg90;

		World::Get().SetItem(newPosition, block);
	}

	void Player::DestroyItem(const glm::vec3& position)
	{
		if (!m_TargetedItem.Targeted)
			return;

		World::Get().SetItem(position, Item(ItemType::Air));
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
