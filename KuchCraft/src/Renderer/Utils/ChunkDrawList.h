#pragma once

#include "Renderer/RendererData.h"

#include "World/Item.h"

#include "Renderer/Renderer2D/Renderer2DData.h"

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

		const std::vector<Quad3DVertex>&            GetFoliageQuadVertices()      const { return m_FoliageQuadVertices;       }
		const std::vector<TransparentQuad3DVertex>& GetTransparentQuadVertices()  const { return m_TransparentQuadVertices;   }
		const std::vector<glm::vec3>&               GetTransparentQuadPositions() const { return m_TransparentQuadsPositions; }

		void NewDrawCall();
		void AddTexture(uint32_t texture);
		void UpdateIndexCount();

		void AddSolid(const glm::ivec3& position, uint32_t verticesIndex, const Item& item);
		void AddTransparent(const glm::vec3& position, const Item& item);
		void AddWater(const glm::vec3& position, uint32_t verticesIndex);
		void AddFoliageQuad(const glm::vec3& position, const Item& item);

	private:
		TextureSlotHelper* m_TextureSlotHelper = nullptr;

		// Solid blocks
		uint32_t m_DrawCalls = 1;
		std::vector<uint32_t> m_IndexCount;
		std::vector<uint32_t> m_Textures;
		std::vector<uint32_t> m_VertexData;

		// FoliageQuads
		std::vector<Quad3DVertex> m_FoliageQuadVertices;
		
		// Transparent stuff
		std::vector<TransparentQuad3DVertex> m_TransparentQuadVertices;
		std::vector<glm::vec3>               m_TransparentQuadsPositions;
	};

}