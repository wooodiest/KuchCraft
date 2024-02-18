#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "World/Block.h"
#include "Renderer/Renderer.h"

namespace KuchCraft {

	constexpr int   chunk_size_XZ = 16;
	constexpr int   chunk_size_Y = 128;

	class Chunk
	{
	public:
		Block Blocks[chunk_size_XZ][chunk_size_Y][chunk_size_XZ];

	public:
		Chunk(const glm::vec3& position);
		~Chunk();

		void Render();
		void Recreate();
		void Build();

		bool NeedToRecreate() const { return m_NeedToRecreate; }
		bool NeedToBuild()    const { return m_NeedToBuild;    }

		void SetRebuildStatus(bool status)  { m_NeedToBuild = status;    }
		void SetRecreateStatus(bool status) { m_NeedToRecreate = status; }

		const glm::vec3& GetPosition()                 const { return m_Position;         }

		std::vector<Vertex>& GetDrawList()                   { return m_DrawList;         }
		std::vector<Vertex>& GetDrawListWater()              { return m_DrawListWater;    }
		RendererChunkData&   GetRendererChunkData()          { return m_RendererChunkData;}

	private:
		glm::vec3 m_Position;
		bool m_NeedToRecreate = true;
		bool m_NeedToBuild    = true;

		std::vector<Vertex>    m_DrawList;
		std::vector<Vertex>    m_DrawListWater;

		RendererChunkData m_RendererChunkData;

	private:
		struct RendererTextureData
		{
			uint32_t TextureSlots[max_texture_slots];
			uint32_t TextureSlotIndex = 0;

			void AddTexture(uint32_t texture)
			{
				TextureSlots[TextureSlotIndex] = texture;
				TextureSlotIndex++;
			}
		};

		void AddToDrawList(RendererTextureData& textureData, const glm::mat4& model, const Vertex vertices[quad_vertex_count], int x, int y, int z);
		void AddToDrawListWater(const glm::mat4& model, const Vertex vertices[quad_vertex_count]);

	};

}