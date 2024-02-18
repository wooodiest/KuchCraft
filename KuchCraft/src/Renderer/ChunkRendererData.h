#pragma once

#include <vector>

#include "Renderer/RendererData.h"

namespace KuchCraft {

	class TextureSlotHelper
	{
	public:
		TextureSlotHelper();

		void ClearSlots();
		void AddTexture(uint32_t texture);

		inline uint32_t GetCurrentSlot()          const { return m_CurrentSlot;          }
		inline uint32_t GetTexture()              const { return m_Slots[m_CurrentSlot]; }
		inline uint32_t GetTexture(uint32_t slot) const { return m_Slots[slot];          }
		
	private:
		uint32_t m_Slots[max_texture_slots];
		uint32_t m_CurrentSlot = 0;

	};

	class ChunkDrawList
	{
	public:
		ChunkDrawList();
		~ChunkDrawList();

		void StartRecreating();
		void EndRecreating();

		// DrawCallIndex starts at 0 and responds to draw call = 1
		inline uint32_t GetDrawCallCount()                                 const { return m_DrawCalls;                                                     } 
		inline uint32_t GetTextureCount(uint32_t drawCallIndex)            const { return (uint32_t)m_Textures.size() - drawCallIndex * max_texture_slots; }
		inline uint32_t GetIndexCount(uint32_t drawCallIndex)              const { return m_IndexCount[drawCallIndex];                                     }
		inline uint32_t GetTexture(uint32_t drawCallIndex, uint32_t index) const { return m_Textures[drawCallIndex * max_texture_slots + index];           }
		inline uint32_t GetCurrentDrawCallIndex()                          const { return m_DrawCalls - 1;                                                 }

		inline TextureSlotHelper& GetSlotHelper() const { return *m_TextureSlotHelper; }

		void NewDrawCall();
		void AddTexture(uint32_t texture);
		void UpdateIndexCount();

	private:
		TextureSlotHelper* m_TextureSlotHelper = nullptr;
		// -> Normal blocks
		uint32_t m_DrawCalls = 1;
		std::vector<uint32_t> m_IndexCount;
		std::vector<uint32_t> m_Textures;

		
	};

}