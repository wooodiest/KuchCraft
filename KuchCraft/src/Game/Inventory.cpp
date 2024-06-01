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

	Inventory::Inventory()
	{
		for (uint32_t i = 0; i < inventory_items_count; i++)
			m_Items[i].Item = Item(ItemType::None);
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
			case KeyCode::D1: m_HotbarSlot = 0; return false;
			case KeyCode::D2: m_HotbarSlot = 1; return false;
			case KeyCode::D3: m_HotbarSlot = 2; return false;
			case KeyCode::D4: m_HotbarSlot = 3; return false;
			case KeyCode::D5: m_HotbarSlot = 4; return false;
			case KeyCode::D6: m_HotbarSlot = 5; return false;
			case KeyCode::D7: m_HotbarSlot = 6; return false;
			case KeyCode::D8: m_HotbarSlot = 7; return false;
			case KeyCode::D9: m_HotbarSlot = 8; return false;

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
		constexpr int8_t min_slot = 0;
		constexpr int8_t max_slot = 8;

		if (e.GetYOffset() > 0)
			m_HotbarSlot++;
		else
			m_HotbarSlot--;

		if (m_HotbarSlot == min_slot - 1)
			m_HotbarSlot = max_slot;
		else if (m_HotbarSlot == max_slot + 1)
			m_HotbarSlot = min_slot;

		return false;
	}

	void Inventory::OnUpdate(float dt)
	{
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();

		Renderer2DQuadInfo hotbarQuadInfo;
		hotbarQuadInfo.Position = { 0.5f * width, 33.0f, 0.0f };
		hotbarQuadInfo.Size     = { 273.0f, 33.0f };
		Renderer2D::DrawQuad(hotbarQuadInfo, AssetManager::GetUIElementTexture(UIElement::Hotbar));

		Renderer2DQuadInfo hotbarSelectionQuadInfo;
		hotbarSelectionQuadInfo.Position = { 0.5f * width + (m_HotbarSlot - 4) * 60.0f, 34.0f, 1.0f };
		hotbarSelectionQuadInfo.Size     = { 33.0f, 33.0f };
		Renderer2D::DrawQuad(hotbarSelectionQuadInfo, AssetManager::GetUIElementTexture(UIElement::HotbarSelected));

		if (m_Open)
		{
			Renderer2DQuadInfo inventoryQuadInfo;
			inventoryQuadInfo.Position = { 0.5f * width, 0.5f * height, 0.09f };
			inventoryQuadInfo.Size     = { inventory_texture_size_x, inventory_texture_size_y };
			Renderer2D::DrawQuad(inventoryQuadInfo, AssetManager::GetUIElementTexture(UIElement::Inventory));
			const Texture2D& slotTexture = AssetManager::GetUIElementTexture(UIElement::Slot);

			const glm::vec2 leftTopCorner     = { 0.5f * width - 4.0f * 2.0f * inventory_cell_size, 0.5f * height - inventory_cell_size };
			const glm::vec2 rightBottomCorner = leftTopCorner + glm::vec2{ 9.0f * 2.0f * inventory_cell_size, -2.0f * 2.0f * inventory_cell_size};
			uint32_t index = 0;

			const glm::vec2 hotbarLeft = { 0.5f * width - 4.0f * 2.0f * inventory_cell_size, 0.5f * height - 2.0f * 3.0f * inventory_cell_size - 11.0f - inventory_cell_size };

			for (float x = hotbarLeft.x; x <= hotbarLeft.x + 9.0f * 2.0f * inventory_cell_size; x += 2.0f * inventory_cell_size)
			{
				Renderer2DQuadInfo exampleQuadInfo;
				exampleQuadInfo.Position = { x, hotbarLeft.y, 0.1f };
				exampleQuadInfo.Size     = { inventory_cell_size, inventory_cell_size };

				Renderer2D::DrawQuad(exampleQuadInfo, slotTexture, index);
				Renderer2D::DrawItem(Item(ItemType((int)ItemType::Dirt + index)), { x, hotbarLeft.y, 0.1f }, index);
				index++;
			}

			int flower = 0; //tmp
			for (float y = leftTopCorner.y; y >= rightBottomCorner.y; y -= 2.0f * inventory_cell_size)
			{
				for (float x = leftTopCorner.x; x <= rightBottomCorner.x; x += 2.0f * inventory_cell_size)
				{
					Renderer2DQuadInfo exampleQuadInfo;
					exampleQuadInfo.Position = { x, y, 0.1f };
					exampleQuadInfo.Size     = { inventory_cell_size, inventory_cell_size };
					Renderer2D::DrawQuad(exampleQuadInfo, slotTexture, index);
					Renderer2D::DrawItem(Item(ItemType((int)ItemType::BeetrootsCrops + flower)), { x, y, 0.1f }, index);

					index++;
					flower++;
				}
			}

		}

	}

	void Inventory::Open()
	{
		m_Open = true;

		Renderer2D::SetShowCursorStatus(true);
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		const glm::vec2 focuedCurosorPosition = { 0.5f * width + (m_HotbarSlot - 4) * inventory_texture_size_x / 4.9f, 0.5f * height - inventory_texture_size_y + inventory_texture_size_y / 5.0f };
		Renderer2D::ResetCursorPosition(focuedCurosorPosition);
	}

	void Inventory::Close()
	{
		m_Open = false;
		Renderer2D::SetShowCursorStatus(false);
	}

	void Inventory::SetItem(const InventoryItem& item)
	{
	}

	void Inventory::SetItem(const Item& item)
	{
		InventoryItem it;
		it.Item  = item;
		it.Count = 1;

		SetItem(it);
	}

	Item Inventory::GetItem(uint32_t slot, bool reduce)
	{
		if (slot >= inventory_items_count)
			return Item(ItemType::None);
		
		Item it = m_Items[slot].Item;

		if (reduce)
		{
			//m_Items[slot].Count--;
		}

		return it;
	}

}

