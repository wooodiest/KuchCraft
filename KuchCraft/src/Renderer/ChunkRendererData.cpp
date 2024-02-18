#include "Renderer/ChunkRendererData.h"

#include <iostream>

namespace KuchCraft {

	ChunkDrawList::ChunkDrawList()
	{
		m_IndexCount.push_back(0);
	}

	ChunkDrawList::~ChunkDrawList()
	{
		m_IndexCount.clear();
		m_Textures.  clear();
	}

	void ChunkDrawList::StartRecreating()
	{
		m_TextureSlotHelper = new TextureSlotHelper();
	}

	void ChunkDrawList::EndRecreating()
	{
		if (m_TextureSlotHelper)
			delete m_TextureSlotHelper;
	}

	void ChunkDrawList::NewDrawCall()
	{
		m_IndexCount.push_back(0);
		m_DrawCalls++;
		m_TextureSlotHelper->ClearSlots();
	}

	void ChunkDrawList::AddTexture(uint32_t texture)
	{
		m_Textures.push_back(texture);
		m_TextureSlotHelper->AddTexture(texture);
	}

	void ChunkDrawList::UpdateIndexCount()
	{
		m_IndexCount[GetCurrentDrawCallIndex()] += quad_index_count;
	}

	///

	TextureSlotHelper::TextureSlotHelper()
	{
		ClearSlots();
	}

	void TextureSlotHelper::ClearSlots()
	{
		std::memset(m_Slots, 0, max_texture_slots * sizeof(uint32_t));
		m_CurrentSlot = 0;
	}

	void TextureSlotHelper::AddTexture(uint32_t texture)
	{
		m_Slots[m_CurrentSlot] = texture;
		m_CurrentSlot++;
	}

}
