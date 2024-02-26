#include "Renderer/RendererChunkData.h"

#include "Renderer/Renderer.h"

namespace KuchCraft {

	ChunkDrawList::ChunkDrawList()
	{
		m_IndexCount.push_back(0);
	}

	ChunkDrawList::~ChunkDrawList()
	{
		m_IndexCount.clear();
		m_Textures.  clear();
		m_Vertices.  clear();

		m_WaterVertices.clear();
	}

	void ChunkDrawList::StartRecreating()
	{
		m_TextureSlotHelper = new TextureSlotHelper();
		m_Vertices.clear();
		m_Vertices.reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_Y * cube_vertex_count);

		m_WaterVertices.clear();
		m_WaterVertices.reserve(chunk_size_XZ * chunk_size_XZ);
	}

	void ChunkDrawList::EndRecreating()
	{
		if (m_TextureSlotHelper)
			delete m_TextureSlotHelper;

		m_Vertices.shrink_to_fit();
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

	void ChunkDrawList::Add(const glm::mat4& model, const Vertex vertices[quad_vertex_count], const Block& block)
	{
		uint32_t texture = Renderer::GetTexture(block.blockType);
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
			if (m_TextureSlotHelper->GetCurrentSlot() == max_texture_slots)
				NewDrawCall();

			texSlot = (float)m_TextureSlotHelper->GetCurrentSlot();
			AddTexture(texture);
		}
		// Create geometry
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			m_Vertices.emplace_back(Vertex{
					glm::vec3(model * glm::vec4(vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z, 1.0f)),
					glm::vec2(vertices[i].TexCoord.x, vertices[i].TexCoord.y),
					texSlot
				});
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
