#include "kcpch.h"
#include "Renderer/Utils/ChunkDrawList.h"

#include "Renderer/Renderer.h"
#include "Renderer/AssetManager.h"
#include "Renderer/Renderer.h"
namespace KuchCraft {

	ChunkDrawList::ChunkDrawList()
	{
	}

	ChunkDrawList::~ChunkDrawList()
	{
		m_IndexCount.clear();
		m_Textures.  clear();
		m_VertexData.clear();

		m_WaterVertices.clear();
	}

	void ChunkDrawList::StartRecreating()
	{
		m_DrawCalls = 1;
		m_TextureSlotHelper = new TextureSlotHelper();

		m_VertexData.clear();
		m_VertexData.reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_Y * cube_vertex_count);

		m_Textures.clear();
		m_Textures.reserve(Renderer::GetInfo().MaxTextureSlots);

		m_IndexCount.clear();
		m_IndexCount.reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_Y * cube_index_count);
		m_IndexCount.push_back(0);

		m_WaterVertices.clear();
		m_WaterVertices.reserve(chunk_size_XZ * chunk_size_XZ);
	}

	void ChunkDrawList::EndRecreating()
	{
		if (m_TextureSlotHelper)
			delete m_TextureSlotHelper;

		m_VertexData.   shrink_to_fit();
		m_IndexCount.   shrink_to_fit();
		m_Textures.     shrink_to_fit();
		m_WaterVertices.shrink_to_fit();
	}

	uint32_t ChunkDrawList::GetTextureCount(uint32_t drawCallIndex) const
	{
		return (uint32_t)m_Textures.size() - drawCallIndex * Renderer::GetInfo().MaxTextureSlots;
	}

	uint32_t ChunkDrawList::GetTexture(uint32_t drawCallIndex, uint32_t index) const
	{
		return m_Textures[drawCallIndex * Renderer::GetInfo().MaxTextureSlots + index]; ;
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

	void ChunkDrawList::Add(const glm::ivec3& position, uint32_t verticesIndex, const Item& item)
	{
		uint32_t texture = AssetManager::GetItemTexture(item.Type).GetRendererID();
		float    texSlot = -1.0f;

		// Check if the texture already has assigned slot
		for (uint32_t slot = 0; slot < m_TextureSlotHelper->GetCurrentSlot(); slot++)
		{
			if (m_TextureSlotHelper->GetTexture(slot) == texture)
			{
				texSlot = (float)slot;
				break;
			}
		}

		// If we haven't found the texture, check whether we have a new one or whether we have used all slots
		if (texSlot == -1.0f)
		{
			if (m_TextureSlotHelper->GetCurrentSlot() == Renderer::GetInfo().MaxTextureSlots)
				NewDrawCall();

			texSlot = (float)m_TextureSlotHelper->GetCurrentSlot();
			AddTexture(texture);
		}

		// Create geometry
		//        PackedData        //
		//   5B - position.x | 0B	//
		//   7B - position.y | 5B	//
		//   5B - position.z | 12B  //
		//	 2B - rotation   | 17B  //
		//   5B - index      | 19B  //
		//   7B - texture    | 24B  //
		uint32_t packedData = 0;
		packedData |= ((position.x & 0x1F)              << 0 ) |
				      ((position.y & 0x7F)              << 5 ) |
				      ((position.z & 0x1F)              << 12) |
					  (((uint8_t)item.Rotation & 0x3)   << 17) |
					  (((uint8_t)texSlot        & 0x7F) << 24);

		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			// every vertex quad has stride of 4
			uint32_t index = verticesIndex * 4 + i;
			uint32_t data  = packedData | ((index & 0x1F) << 19);

			m_VertexData.push_back(data);
		}
		UpdateIndexCount();
	}

	void ChunkDrawList::AddWater(const glm::mat4& model, const Vertex vertices[quad_vertex_count])
	{
		for (int i = 0; i < quad_vertex_count; i++)
		{
			m_WaterVertices.emplace_back(Vertex_P3C2{
					glm::vec3(model * glm::vec4(vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z, 1.0f)),
					glm::vec2(vertices[i].TexCoord.x, vertices[i].TexCoord.y)
				});
		}
	}

	///

	TextureSlotHelper::TextureSlotHelper()
	{
		m_Slots = new uint32_t[Renderer::GetInfo().MaxTextureSlots];
		ClearSlots();
	}

	TextureSlotHelper::~TextureSlotHelper()
	{
		delete[] m_Slots;
	}

	void TextureSlotHelper::ClearSlots()
	{
		for (uint32_t i = 0; i < Renderer::GetInfo().MaxTextureSlots; i++)
			m_Slots[i] = 0;
		m_CurrentSlot = 0;
	}

	void TextureSlotHelper::AddTexture(uint32_t texture)
	{
		m_Slots[m_CurrentSlot] = texture;
		m_CurrentSlot++;
	}

}
