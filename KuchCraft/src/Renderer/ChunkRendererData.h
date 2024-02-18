#pragma once

#include <vector>

#include "Renderer/RendererData.h"

namespace KuchCraft {

	struct TextureSlotHelper
	{

	};

	class ChunkDrawList
	{
	public:
		ChunkDrawList();
		~ChunkDrawList();

		// DrawCallIndex starts at 0 and responds to draw call - 1
		inline uint32_t GetDrawCallCount()                                 const { return m_DrawCalls;                                                     } 
		inline uint32_t GetTextureCount(uint32_t drawCallIndex)            const { return (uint32_t)m_Textures.size() - drawCallIndex * max_texture_slots; }
		inline uint32_t GetIndexCount(uint32_t drawCallIndex)              const { return m_IndexCount[drawCallIndex];                                     }
		inline uint32_t GetTexture(uint32_t drawCallIndex, uint32_t index) const { return m_Textures[drawCallIndex * max_texture_slots + index];           }

		inline uint32_t GetCurrentTextureIndex()          const { return m_SlotIndex;          } 
		inline uint32_t GetCurrentTexture()               const { return m_Slots[m_SlotIndex]; }
		inline uint32_t GetCurrentTexture(uint32_t index) const { return m_Slots[index];       }

		void NewDrawCall();
		void AddTexture(uint32_t texture);
		void UpdateIndexCount();


	private:
		// -> Normal blocks
		uint32_t m_DrawCalls = 1;
		std::vector<uint32_t> m_IndexCount;
		std::vector<uint32_t> m_Textures;

		// Utils
		uint32_t m_Slots[max_texture_slots];
		uint32_t m_SlotIndex = 0;
		void ClearSlots();

	};

}