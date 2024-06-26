#pragma once

#include "Renderer/Data/Camera.h"

#include "Physics/Ray.h"

#include "World/Item.h"

#include "Physics/PlayerPhysicsBody.h"

#include "Core/Input.h"
#include "Core/Events/Event.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

#include "Game/Inventory.h"

namespace KuchCraft {

	constexpr float player_height          = 1.8f;
	constexpr float player_eye_height      = 1.7f;
	constexpr float player_width           = 0.6f;

	enum class GameMode
	{
		Survival  = 0,
		Creative  = 1,
		Spectator = 2
	};

	class Player
	{
	public:
		Player();
		Player(const glm::vec3& position, const glm::vec2& rotation, GameMode gameMode);
		~Player();

		void OnUpdate(float dt);
		void OnEvent(Event& event);

		Camera& GetCamera() { return m_Camera; }
		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec2& GetRotation() const { return m_Rotation; }

		std::string& GetDebugText();

		bool GetTargetedBlockStatus()             const { return m_TargetedItem.Targeted; }
		const TargetedItemInfo& GetTargetedItem() const { return m_TargetedItem;          }

		void SetPosition(const glm::vec3& position) { m_Position = position; }
		void SetRotation(const glm::vec2& rotation) { m_Rotation = rotation; }

		const glm::vec3 GetHeadPosition() const { return glm::vec3(m_Position.x, m_Position.y + player_height,     m_Position.z); }
		const glm::vec3 GetEyePosition()  const { return glm::vec3(m_Position.x, m_Position.y + player_eye_height, m_Position.z); }

		void SetGameMode(GameMode gameMode);

	private:
		void Init();
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void PlaceItem(const glm::vec3& position, Item& item);
		void DestroyItem(const glm::vec3& position);

		TargetedItemInfo GetTargetItemInfo();

		void ShowExampleUI();
		void ShowDebugText();

	private:
		glm::vec3 m_Position{ 0.0f };
		glm::vec2 m_Rotation{ 0.0f }; // yaw, pitch
		glm::vec2 m_PrevMousePosition{ 0.0f };

		Camera			  m_Camera;
		GameMode		  m_GameMode;
		PlayerPhysicsBody m_PhysicsBody;
		TargetedItemInfo  m_TargetedItem;
		Inventory         m_Inventory;

		std::string m_DebugText;
		bool m_ShowDebugText = true;

	};

}