#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Renderer/Renderer.h"

namespace KuchCraft {

	constexpr int world_chunk_size = 100; // square: world_chunk_size x world_chunk_size
	constexpr int chunk_size_XZ	   = 16;
	constexpr int chunk_size_Y	   = 128;

	enum class BlockType
	{
		Air = 0, /*Water,*/
		Bedrock, Bricks,
		CoalOre, Cobblestone, CraftingTable,
		DiamondOre, Dioryte, Dirt,
		Furnace,
		Granite, Grass, Gravel,
		IronOre,
		OakLog, OakPlanks,
		Sand, Stone, StoneBrick,

		LastElement // temporary
	};

	class Block
	{
	public:
		BlockType blockType = BlockType::Air;
		static bool IsTranspaent(const Block& block);

		operator BlockType()                     const { return  blockType;                    }
		bool operator ==(const Block& other)     const { return  blockType == other.blockType; }
		bool operator !=(const Block& other)     const { return !(*this    == other);          }
		bool operator ==(const BlockType& other) const { return  blockType == other;           }
		bool operator !=(const BlockType& other) const { return !(*this    == other);          }
	};

	class Chunk
	{
	public:
		Chunk(const glm::vec3& position);

		Block blocks[chunk_size_XZ][chunk_size_Y][chunk_size_XZ];

		void Render();
		void Recreate();
		void Build();

		bool NeedToRecreate() const { return m_NeedToRecreate; }
		bool NeedToBuild()    const { return m_NeedToBuild;    }

		const glm::vec3&              GetPosition()    const { return m_Position;         }
		const std::vector<BlockType>& GetTextureList() const { return m_DrawListTextures; }
		std::vector<Vertex>&          GetDrawList()          { return m_DrawList;         }

		// Temporary
		void FillWithRandomBlocks();
		void FillWithOneBlock(); 

	private:
		bool m_NeedToRecreate = true;
		bool m_NeedToBuild    = true;
		glm::vec3 m_Position;

		std::vector<Vertex>    m_DrawList;
		std::vector<BlockType> m_DrawListTextures;

		void AddToDrawList(const glm::mat4& model, const Vertex vertices[4], int x, int y, int z);

	};

	class World
	{
	public:	
		static void Init();
		static void Shutdown();

		static void OnUpdate(float dt, const glm::vec3& position);
		static void Render();

		static glm::vec2 GetChunkIndex(const glm::vec3& position);
		static Chunk*    GetChunk(const glm::vec3& position);
		static std::vector<Chunk*>& GetChunks()       { return s_Chunks; }
		static std::vector<Chunk*>& GetChunksToDraw() { return s_ChunksToDraw; }

		static const glm::vec3& CalculateChunkAbsolutePosition(const glm::vec3& position);

	private:
		static uint32_t            s_RenderDistance;
		static std::vector<Chunk*> s_Chunks;
		static std::vector<Chunk*> s_ChunksToDraw;

		World() = default;
	};

}