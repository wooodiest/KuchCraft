#pragma once

#include "Renderer/RendererData.h"

#include "World/Item.h"

namespace KuchCraft {

	class TextureSlotHelper
	{
	public:
		TextureSlotHelper();
		~TextureSlotHelper();

		void ClearSlots();
		void AddTexture(uint32_t texture);

		uint32_t GetCurrentSlot()          const { return m_CurrentSlot;          }
		uint32_t GetTexture()              const { return m_Slots[m_CurrentSlot]; }
		uint32_t GetTexture(uint32_t slot) const { return m_Slots[slot];          }
		
	private:
		uint32_t* m_Slots;
		uint32_t  m_CurrentSlot = 0;

	};

	class ChunkDrawList
	{
	public:
		ChunkDrawList();
		~ChunkDrawList();

		void StartRecreating();
		void EndRecreating();

		// DrawCallIndex starts at 0 and responds to draw call = 1
		uint32_t GetDrawCallCount()                                 const { return m_DrawCalls;                 } 
		uint32_t GetTextureCount(uint32_t drawCallIndex)            const;
		uint32_t GetIndexCount(uint32_t drawCallIndex)              const { return m_IndexCount[drawCallIndex]; }
		uint32_t GetTexture(uint32_t drawCallIndex, uint32_t index) const; 
		uint32_t GetCurrentDrawCallIndex()                          const { return m_DrawCalls - 1;             }

		TextureSlotHelper&           GetSlotHelper()  const { return *m_TextureSlotHelper; }
		const std::vector<uint32_t>& GetVertices()    const { return m_VertexData;         }
		const void* GetVerticesPtr(uint32_t offset)   const { return &m_VertexData[offset];}

		const std::vector<Vertex_P3C2>& GetWaterVertices() const { return m_WaterVertices;     }
		const void* GetWaterVerticesPtr()                  const { return &m_WaterVertices[0]; }

		void NewDrawCall();
		void AddTexture(uint32_t texture);
		void UpdateIndexCount();

		void Add(const glm::ivec3& position, uint32_t verticesIndex, const Item& block);
		void AddWater(const glm::mat4& model, const Vertex vertices[quad_vertex_count]);

	private:
		TextureSlotHelper* m_TextureSlotHelper = nullptr;
		// -> Normal blocks
		uint32_t m_DrawCalls = 1;
		std::vector<uint32_t> m_IndexCount;
		std::vector<uint32_t> m_Textures;
		std::vector<uint32_t> m_VertexData;

		// Water
		std::vector<Vertex_P3C2> m_WaterVertices;
		
	};

}