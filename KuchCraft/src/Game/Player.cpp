#include "kcpch.h"
#include "Game/Player.h"

#include "World/World.h"

#include "Renderer/Renderer.h"
#include "Renderer/Renderer3D/Renderer3D.h"
#include "Renderer/Renderer2D/Renderer2D.h"
#include "Renderer/AssetManager.h"

#include "Core/Application.h"

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

		ShowExampleUI();
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
			case GameMode::Survival:  gameMode = "Survival";  break;
			case GameMode::Creative:  gameMode = "Creative";  break;
			case GameMode::Spectator: gameMode = "Spectator"; break;
		}

		m_DebugText =
			  "\nPlayer:"
			  "\n   Position: "  + Utils::VecToString(m_Position, 2)
			+ "\n   Rotation: "  + Utils::VecToString(glm::vec2{ glm::degrees(m_Rotation.x), glm::degrees(m_Rotation.y) }, 2)
			+ "\n   Gamemode: "  + gameMode;

		if (m_TargetedItem.Targeted)
			m_DebugText += "\n   Targeted item - " + World::Get().GetItem(m_TargetedItem.Position).GetName() + " : " + Utils::VecToString(glm::ivec3{m_TargetedItem.Position});
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

	void Player::ShowExampleUI()
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();

#if 0
		{
			Renderer2DQuadInfo textDebugQuadInfo;
			textDebugQuadInfo.Position = { 150.0f, height - 150.0f, 0.0f };
			textDebugQuadInfo.Size     = { 150.0f, 150.0f };
			Renderer2D::DrawQuad(textDebugQuadInfo, { 1.0f, 1.0f, 1.0f, 0.5f });
		}
#endif
		if (m_GameMode != GameMode::Spectator)
		{
			Renderer2DQuadInfo hotbarQuadInfo;
			hotbarQuadInfo.Position = { 0.5f, 33.0f, 0.0f };
			hotbarQuadInfo.Size     = { 273.0f, 33.0f };
			hotbarQuadInfo.NormalizedPosition_X = true;
			Renderer2D::DrawQuad(hotbarQuadInfo, AssetManager::GetUIElementTexture(UIElement::Hotbar));

			Renderer2DQuadInfo hotbarSelectionQuadInfo;
			hotbarSelectionQuadInfo.Position = { 0.5f * width - 3 * 60.0f, 34.0f, 1.0f };
			hotbarSelectionQuadInfo.Size     = { 33.0f, 33.0f };
			Renderer2D::DrawQuad(hotbarSelectionQuadInfo, AssetManager::GetUIElementTexture(UIElement::HotbarSelected));

			Renderer2DQuadInfo crosshairQuadInfo;
			crosshairQuadInfo.Position = { 0.5f, 0.5f, 1.0f };
			crosshairQuadInfo.Size     = { 15.0f, 15.0f };
			crosshairQuadInfo.NormalizedPosition_X = true;
			crosshairQuadInfo.NormalizedPosition_Y = true;
			Renderer2D::DrawQuad(crosshairQuadInfo, AssetManager::GetUIElementTexture(UIElement::Crosshair));
		}

		if (m_GameMode == GameMode::Survival)
		{
			Renderer2DQuadInfo fullHeartQuadInfo;
			fullHeartQuadInfo.Size = { 13.0f, 13.0f };
			Texture2D& fullHeartTexture = AssetManager::GetUIElementTexture(UIElement::HeartFull);

			Renderer2DQuadInfo emptyHeartQuadInfo;
			emptyHeartQuadInfo.Size = { 10.0f, 10.0f };
			Texture2D& emptyHeartTexture = AssetManager::GetUIElementTexture(UIElement::HeartEmpty);

			glm::vec3 initialHeartPosition = { 0.5f * width - 273.0f, 80.0f, 1.0f };

			constexpr uint32_t heart_count      = 10;
			constexpr uint32_t full_heart_count = 6;

			for (uint32_t i = 0; i < heart_count; i++)
			{
				if (i < full_heart_count)
				{
					fullHeartQuadInfo.Position = { initialHeartPosition.x + i * 2 * 11.0f + 13.0f, initialHeartPosition.y , 1.0f };
					Renderer2D::DrawQuad(fullHeartQuadInfo, fullHeartTexture);
				}
				else
				{
					emptyHeartQuadInfo.Position = { initialHeartPosition.x + i * 2 * 11.0f + 13.0f, initialHeartPosition.y , 1.0f };
					Renderer2D::DrawQuad(emptyHeartQuadInfo, emptyHeartTexture);
				}
			}
		
			Renderer2DQuadInfo fullArmorQuadInfo;
			fullArmorQuadInfo.Size = { 10.0f, 10.0f };
			Texture2D& fullArmorTexture = AssetManager::GetUIElementTexture(UIElement::ArmorFull);

			Renderer2DQuadInfo emptyArmorQuadInfo;
			emptyArmorQuadInfo.Size = { 10.0f, 10.0f };
			Texture2D& emptyArmorTexture = AssetManager::GetUIElementTexture(UIElement::ArmorEmpty);

			glm::vec3 initialArmorPosition = { 0.5f * width - 273.0f, 105.0f, 1.0f };

			constexpr uint32_t armor_count      = 10;
			constexpr uint32_t full_armor_count = 3;

			for (uint32_t i = 0; i < armor_count; i++)
			{
				if (i < full_armor_count)
				{
					fullArmorQuadInfo.Position = { initialArmorPosition.x + i * 2 * 11.0f + 13.0f, initialArmorPosition.y , 1.0f };
					Renderer2D::DrawQuad(fullArmorQuadInfo, fullArmorTexture);
				}
				else
				{
					emptyArmorQuadInfo.Position = { initialArmorPosition.x + i * 2 * 11.0f + 13.0f, initialArmorPosition.y , 1.0f };
					Renderer2D::DrawQuad(emptyArmorQuadInfo, emptyArmorTexture);
				}
			}
		
			Renderer2DQuadInfo fullFoodQuadInfo;
			fullFoodQuadInfo.Size = { 13.0f, 13.0f };
			Texture2D& fullFoodTexture = AssetManager::GetUIElementTexture(UIElement::FoodFull);

			Renderer2DQuadInfo emptyFoodQuadInfo;
			emptyFoodQuadInfo.Size = { 10.0f, 10.0f };
			Texture2D& emptyFoodTexture = AssetManager::GetUIElementTexture(UIElement::FoodEmpty);

			glm::vec3 initialFoodPosition = { 0.5f * width + 273.0f, 80.0f, 1.0f };

			constexpr uint32_t food_count      = 10;
			constexpr uint32_t full_food_count = 6;

			for (uint32_t i = 0; i < food_count; i++)
			{
				if (i < full_food_count)
				{
					fullFoodQuadInfo.Position = { initialFoodPosition.x - i * 2 * 11.0f - 13.0f, initialFoodPosition.y , 1.0f };
					Renderer2D::DrawQuad(fullFoodQuadInfo, fullFoodTexture);
				}
				else
				{
					emptyFoodQuadInfo.Position = { initialFoodPosition.x - i * 2 * 11.0f - 13.0f, initialFoodPosition.y , 1.0f };
					Renderer2D::DrawQuad(emptyFoodQuadInfo, emptyFoodTexture);
				}
			}
		}
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
					PlaceItem(m_TargetedItem.Position, Item((ItemType)Random::UInt(1, item_types_count - 1)));
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
			case GameMode::Survival:
			{
				m_PhysicsBody.SetFlyingStatus(false);
				m_PhysicsBody.SetCollidingStatus(true);
				break;
			}
			case GameMode::Creative:
			{
				m_PhysicsBody.SetCollidingStatus(true);
				break;
			}
			case GameMode::Spectator:
			{
				m_PhysicsBody.SetFlyingStatus(true);
				m_PhysicsBody.SetCollidingStatus(false);
				break;
			}
		}
	}

}
