#pragma once

#include "World/Item.h"

#include "Core/Events/Event.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

namespace KuchCraft {

	constexpr uint32_t inventory_items_count = 36;

	struct InventoryItem
	{
		Item     Item;
		uint32_t Count = 1;
	};

	class Inventory
	{
	public:
		Inventory();
		~Inventory();

		void OnEvent(Event& event);
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
		void OnUpdate(float dt);

		void Open();
		void Close();

		bool GetOpenStatus() const { return m_Open; }

		void SetItem(const InventoryItem& item);
		void SetItem(const Item& item);

		Item GetItem(uint32_t slot, bool reduce = true);

	private:
		void DrawInventoryItem(uint32_t index, const glm::vec3& position); // todo: do map, index to position
		void DrawHotbarItem   (uint32_t index, const glm::vec3& position); // todo: do map, index to position

	private:
		bool m_Open = false;
		InventoryItem m_Items[inventory_items_count];

		int8_t m_HotbarSlot = 0;

	};

}