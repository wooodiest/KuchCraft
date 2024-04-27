#include "kcpch.h"
#include "Renderer/Utils/ChunkDrawList.h"

#include "Renderer/Renderer.h"
#include "Renderer/AssetManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer3D/Renderer3DData.h"

namespace KuchCraft {

	ChunkDrawList::ChunkDrawList()
	{
	}

	ChunkDrawList::~ChunkDrawList()
	{
		m_IndexCount.clear();
		m_Textures.  clear();
		m_VertexData.clear();

		m_FoliageQuadVertices.clear();

		m_TransparentQuadVertices  .clear();
		m_TransparentQuadsPositions.clear();
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
		m_IndexCount.reserve(3);
		m_IndexCount.push_back(0);

		m_FoliageQuadVertices.clear();
		m_FoliageQuadVertices.reserve(chunk_size_XZ * chunk_size_XZ * quad_vertex_count);

		m_TransparentQuadVertices.clear();
		m_TransparentQuadVertices.reserve(chunk_size_XZ * chunk_size_XZ * quad_vertex_count);
		m_TransparentQuadsPositions.clear();
		m_TransparentQuadsPositions.reserve(chunk_size_XZ * chunk_size_XZ * quad_vertex_count);
	}

	void ChunkDrawList::EndRecreating()
	{
		if (m_TextureSlotHelper)
			delete m_TextureSlotHelper;

		m_VertexData.   shrink_to_fit();
		m_IndexCount.   shrink_to_fit();
		m_Textures.     shrink_to_fit();
		m_FoliageQuadVertices      .shrink_to_fit();
		m_TransparentQuadsPositions.shrink_to_fit();
		m_TransparentQuadsPositions.shrink_to_fit();
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

	void ChunkDrawList::AddSolid(const glm::ivec3& position, uint32_t verticesIndex, const Item& item)
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

	void ChunkDrawList::AddTransparent(const glm::vec3& position, const Item& item)
	{
		float texture = AssetManager::GetItemTexture(item.Type).GetRendererID();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

		for (uint32_t i = 0; i < cube_vertex_count; i++)
		{
			TransparentQuad3DVertex vertex;
			vertex.Position = transform * glm::vec4(block_vertices[i].Position, 1.0f);
			vertex.TexCoord = block_vertices[i].TexCoord;
			vertex.TexIndex = texture;

			m_TransparentQuadVertices.emplace_back(vertex);
		}

		for (uint32_t i = 0; i < cube_vertex_count / quad_vertex_count; i++)
			m_TransparentQuadsPositions.emplace_back(position + block_quad_centers[i]);
	}

	void ChunkDrawList::AddWater(const glm::vec3& position, uint32_t verticesIndex)
	{
		float texture = AssetManager::GetItemTexture(ItemType::Water).GetRendererID();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			TransparentQuad3DVertex vertex;
			vertex.Position = transform * glm::vec4(block_vertices[verticesIndex * quad_vertex_count + i].Position, 1.0f);
			vertex.TexCoord = block_vertices[verticesIndex * quad_vertex_count + i].TexCoord;
			vertex.TexIndex = texture;

			m_TransparentQuadVertices.emplace_back(vertex);
		}

		m_TransparentQuadsPositions.emplace_back(position + block_quad_centers[verticesIndex]);
	}

	void ChunkDrawList::AddFoliageQuad(const glm::vec3& position, const Item& item)
	{
		float texture     = AssetManager::GetItemTexture(item.Type).GetRendererID();
		glm::vec3 offsets = item.GetPositionAndRotationOffset(position);

		// quad -> 1
		glm::mat4 rotation1      = glm::toMat4(glm::quat(glm::vec3({ 0.0f, glm::radians(45.0f) + offsets.y, 0.0f})));
		glm::mat4 quadTransform1 = glm::translate(glm::mat4(1.0f), position + glm::vec3{ 0.5f + offsets.x, 0.5f, 0.5f + offsets.z }) *
			rotation1 *
			glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 1.0f });

		glm::vec3 normal1 = rotation1 * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			Quad3DVertex vertex;
			vertex.Position = quadTransform1 * quad_vertex_positions[i];
			vertex.Normal   = normal1;
			vertex.TexCoord = quad_vertex_tex_coords[i];
			vertex.TexIndex = texture;

			m_FoliageQuadVertices.emplace_back(vertex);
		}

		// quad -> 2
		glm::mat4 rotation2      = glm::toMat4(glm::quat(glm::vec3({ 0.0f, glm::radians(45.0f + 90.0f) + offsets.y, 0.0f })));
		glm::mat4 quadTransform2 = glm::translate(glm::mat4(1.0f), position + glm::vec3{ 0.5f + offsets.x, 0.5f, 0.5f + offsets.z }) *
			rotation2 *
			glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 1.0f });

		glm::vec3 normal2 = rotation2 * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			Quad3DVertex vertex;
			vertex.Position = quadTransform2 * quad_vertex_positions[i];
			vertex.Normal   = normal2;
			vertex.TexCoord = quad_vertex_tex_coords[i];
			vertex.TexIndex = texture;

			m_FoliageQuadVertices.emplace_back(vertex);
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
