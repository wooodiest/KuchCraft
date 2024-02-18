#include "Renderer/ChunkRendererData.h"

#include <iostream>

namespace KuchCraft {

	ChunkDrawList::ChunkDrawList()
	{
		m_IndexCount.push_back(0);
		ClearSlots();
	}

	ChunkDrawList::~ChunkDrawList()
	{
		m_IndexCount.clear();
		m_Textures.  clear();
	}

	void ChunkDrawList::NewDrawCall()
	{
		m_IndexCount.push_back(0);
		m_DrawCalls++;
		ClearSlots();
	}

	void ChunkDrawList::AddTexture(uint32_t texture)
	{
		m_Textures.push_back(texture);
		m_Slots[m_SlotIndex] = texture;
		m_SlotIndex++;
	}

	void ChunkDrawList::UpdateIndexCount()
	{
		m_IndexCount[m_DrawCalls - 1] += quad_index_count;
	}

	void ChunkDrawList::ClearSlots()
	{
		std::memset(m_Slots, 0, max_texture_slots * sizeof(uint32_t));
		m_SlotIndex = 0;
	}

}
