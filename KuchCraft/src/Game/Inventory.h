#pragma once

#include "World/Item.h"

#include "Core/Events/Event.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

namespace KuchCraft {

	constexpr uint32_t inventory_items_count = 36;

	enum class InventoryItemSlot : uint8_t
	{
		HotbarSlot1 = 0, HotbarSlot2, HotbarSlot3, HotbarSlot4, HotbarSlot5, HotbarSlot6, HotbarSlot7, HotbarSlot8, HotbarSlot9,
		Slot1 , Slot2 , Slot3 , Slot4 , Slot5 , Slot6 , Slot7 , Slot8 , Slot9,
		Slot10, Slot11, Slot12, Slot13, Slot14, Slot15, Slot16, Slot17, Slot18,
		Slot19, Slot20, Slot21, Slot22, Slot23, Slot24, Slot25, Slot26, Slot27
	};

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

	private:
		void DrawInventoryItem(InventoryItemSlot slot);
		void DrawHotbarItem   (InventoryItemSlot slot);

	private:
		bool m_Open = false;
		InventoryItem m_Items[inventory_items_count];

		InventoryItemSlot m_HotbarSlot = InventoryItemSlot::HotbarSlot1;

	};

}