#include "kcpch.h"
#include "Inventory.h"

#include "Core/Application.h"
#include "Renderer/Renderer2D/Renderer2D.h"
#include "Renderer/AssetManager.h"

namespace KuchCraft {

	constexpr float inventory_texture_size_muliplier = 1.3f;
	constexpr float inventory_texture_size_x = 176.0f * inventory_texture_size_muliplier;
	constexpr float inventory_texture_size_y = 166.0f * inventory_texture_size_muliplier;
	constexpr float inventory_cell_size = 18.0f * inventory_texture_size_muliplier;
	constexpr float hotbar_texture_slot_size = 33.0f;

	static glm::vec2 GetInventorySlotPosition(InventoryItemSlot slot)
	{
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 pos = { 0.0f, 0.0f };

		uint8_t i = (uint8_t)slot;
	
		if (i >= (uint8_t)InventoryItemSlot::HotbarSlot1 && i <= (uint8_t)InventoryItemSlot::HotbarSlot9)
		{
			pos.x = 0.5f * width + (i - 4) * 2.0f * inventory_cell_size;
			pos.y = 0.5f * height - 2.0f * 3.0f * inventory_cell_size - 11.0f - inventory_cell_size;
		}

		if (i >= (uint8_t)InventoryItemSlot::Slot1 && i <= (uint8_t)InventoryItemSlot::Slot27)
		{
			i -= (uint8_t)InventoryItemSlot::Slot1;

			pos.x = 0.5f * width + ((i % 9) - 4) * 2.0f * inventory_cell_size;
			pos.y = 0.5f * height - inventory_cell_size - (i / 9) *  2.0f * inventory_cell_size;
		}

		return pos;
	}

	Inventory::Inventory()
	{
		for (uint32_t i = 0; i < inventory_items_count; i++)
		{
			m_Items[i].Item = Item((ItemType)Random::UInt(1, item_types_count - 1));
			m_Items[i].Count = Random::UInt(1, 64);
		}
	}

	Inventory::~Inventory()
	{
	}

	void Inventory::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(KC_BIND_EVENT_FN(Inventory::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(KC_BIND_EVENT_FN(Inventory::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseScrolledEvent>(KC_BIND_EVENT_FN(Inventory::OnMouseScrolled));
	}

	bool Inventory::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		switch (e.GetKeyCode())
		{
			case KeyCode::D1: m_HotbarSlot = InventoryItemSlot::HotbarSlot1; return false;
			case KeyCode::D2: m_HotbarSlot = InventoryItemSlot::HotbarSlot2; return false;
			case KeyCode::D3: m_HotbarSlot = InventoryItemSlot::HotbarSlot3; return false;
			case KeyCode::D4: m_HotbarSlot = InventoryItemSlot::HotbarSlot4; return false;
			case KeyCode::D5: m_HotbarSlot = InventoryItemSlot::HotbarSlot5; return false;
			case KeyCode::D6: m_HotbarSlot = InventoryItemSlot::HotbarSlot6; return false;
			case KeyCode::D7: m_HotbarSlot = InventoryItemSlot::HotbarSlot7; return false;
			case KeyCode::D8: m_HotbarSlot = InventoryItemSlot::HotbarSlot8; return false;
			case KeyCode::D9: m_HotbarSlot = InventoryItemSlot::HotbarSlot9; return false;

			case KeyCode::E:
			{
				if (m_Open)
					Close();
				else
					Open();

				return false;
			}

		}
		return false;
	}

	bool Inventory::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		return false;
	}

	bool Inventory::OnMouseScrolled(MouseScrolledEvent& e)
	{
		if (e.GetYOffset() > 0)
		{
			if (m_HotbarSlot == InventoryItemSlot::HotbarSlot9)
				m_HotbarSlot = InventoryItemSlot::HotbarSlot1;
			else
				m_HotbarSlot = (InventoryItemSlot)((int8_t)m_HotbarSlot + 1);
		}
		else
		{
			if (m_HotbarSlot == InventoryItemSlot::HotbarSlot1)
				m_HotbarSlot = InventoryItemSlot::HotbarSlot9;
			else
				m_HotbarSlot = (InventoryItemSlot)((int8_t)m_HotbarSlot - 1);
		}

		return false;
	}

	void Inventory::OnUpdate(float dt)
	{
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();

		// main hotbar
		{
			Renderer2DQuadInfo hotbar;
			hotbar.Position = { 0.5f * width, 33.0f, 0.0f };
			hotbar.Size     = { 273.0f, 33.0f };
			Renderer2D::DrawQuad(hotbar, AssetManager::GetUIElementTexture(UIElement::Hotbar));

			Renderer2DQuadInfo hotbarSelected;
			hotbarSelected.Position = { 0.5f * width + ((int)m_HotbarSlot - 4) * 60.0f, 34.0f, 1.0f };
			hotbarSelected.Size     = { 33.0f, 33.0f };
			Renderer2D::DrawQuad(hotbarSelected, AssetManager::GetUIElementTexture(UIElement::HotbarSelected));

			for (uint8_t i = (uint8_t)InventoryItemSlot::HotbarSlot1; i <= (uint8_t)InventoryItemSlot::HotbarSlot9; i++)
				DrawHotbarItem((InventoryItemSlot)i);

		}
		
		if (m_Open)
		{
			Renderer2DQuadInfo inventory;
			inventory.Position = { 0.5f * width, 0.5f * height, 0.09f };
			inventory.Size     = { inventory_texture_size_x, inventory_texture_size_y };
			Renderer2D::DrawQuad(inventory, AssetManager::GetUIElementTexture(UIElement::Inventory));

			for (uint8_t i = (uint8_t)InventoryItemSlot::HotbarSlot1; i <= (uint8_t)InventoryItemSlot::HotbarSlot9; i++)
				DrawInventoryItem((InventoryItemSlot)i);

			for (uint8_t i = (uint8_t)InventoryItemSlot::Slot1; i <= (uint8_t)InventoryItemSlot::Slot27; i++)
				DrawInventoryItem((InventoryItemSlot)i);			
		}

	}

	void Inventory::Open()
	{
		m_Open = true;

		Renderer2D::SetShowCursorStatus(true);
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		const glm::vec2 focuedCurosorPosition = { 0.5f * width + ((int)m_HotbarSlot - 4) * inventory_texture_size_x / 4.9f, 0.5f * height - inventory_texture_size_y + inventory_texture_size_y / 5.0f };
		Renderer2D::ResetCursorPosition(focuedCurosorPosition);
	}

	void Inventory::DrawInventoryItem(InventoryItemSlot slot)
	{
		const InventoryItem& item = m_Items[(uint32_t)slot];
		Renderer2DID id = (uint32_t)slot;

		const glm::vec3  position    = glm::vec3(GetInventorySlotPosition(slot), 0.1f);
		const Texture2D& slotTexture = AssetManager::GetUIElementTexture(UIElement::Slot);

		Renderer2DQuadInfo info;
		info.Position = position;
		info.Size     = { inventory_cell_size, inventory_cell_size };

		Renderer2D::DrawQuad(info, slotTexture, id);
		Renderer2D::DrawItem(item.Item, position, id);

		TextStyle2D style;
		style.FontSize = 16.0f;
		style.Position = { position.x + 10.0f, position.y - 7.0f, 0.11f };
		if (item.Count > 9)
			style.Position.x -= 10.0f;
		Renderer2D::DrawText(std::to_string(item.Count), style);
	}

	void Inventory::DrawHotbarItem(InventoryItemSlot slot)
	{
		const InventoryItem& item = m_Items[(uint32_t)slot];
		Renderer2DID id = (uint32_t)slot;

		glm::vec3 position = { 0.5f * Application::Get().GetWindow().GetWidth() + ((float)slot - 4.0f) * 60.0f, 34.0f, 0.1f};

		Renderer2DQuadInfo info;
		info.Position = position;
		info.Size     = { hotbar_texture_slot_size, hotbar_texture_slot_size };

		Renderer2D::DrawItem(item.Item, position, id);

		TextStyle2D style;
		style.FontSize = 16.0f;
		style.Position = { position.x + 10.0f, position.y - 7.0f, 0.11f };
		if (item.Count > 9)
			style.Position.x -= 10.0f;
		Renderer2D::DrawText(std::to_string(item.Count), style);
	}

	void Inventory::Close()
	{
		m_Open = false;
		Renderer2D::SetShowCursorStatus(false);
	}

}

