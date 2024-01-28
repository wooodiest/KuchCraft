#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "Renderer/Renderer.h"
#include "Game/Player.h"

namespace KuchCraft {

	constexpr int world_chunk_size     = 1000; // square: world_chunk_size x world_chunk_size
	constexpr int chunk_size_XZ	       = 16;
	constexpr int chunk_size_Y	       = 128;

	enum class BlockType
	{
		Air = 0,
		Bedrock, Bricks,
		CoalOre, Cobblestone, CraftingTable,
		DiamondOre, Dioryte, Dirt,
		Furnace,
		Granite, Grass, Gravel,
		IronOre,
		OakLog, OakPlanks,
		Sand, Stone, StoneBrick,

		None // No element and/or last element
	};

	class Block
	{
	public:
		Block()  = default;
		Block(const BlockType& type);
		~Block() = default;

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

		const glm::vec3&              GetPosition()    const { return m_Position;         }
		const std::vector<BlockType>& GetTextureList() const { return m_DrawListTextures; }
		std::vector<Vertex>&          GetDrawList()          { return m_DrawList;         }

	private:
		glm::vec3 m_Position;
		bool m_NeedToRecreate = true;
		bool m_NeedToBuild    = true;

		std::vector<Vertex>    m_DrawList;
		std::vector<BlockType> m_DrawListTextures;

	private:
		void AddToDrawList(const glm::mat4& model, const Vertex vertices[quad_vertex_count], int x, int y, int z);

	};

	class World
	{
	public:	
		World(const std::string& path);
		~World();

		void Shutdown();
		inline static World& Get() { return *s_Instance; }

		void OnUpdate(float dt);
		void Render();

		void  SetBlock(const glm::vec3& position, const Block& block);
		Block GetBlock(const glm::vec3& position);

		const Camera& GetCamera() const { return m_Player.GetCamera(); }
		bool GetQuitStatus()      const { return m_QuitStatus;         }

		int    GetChunkIndex(const glm::vec3& position);
		Chunk* GetChunk(const glm::vec3& position);
		Chunk* GetChunkToRecreate(const glm::vec3& position);

		std::vector<Chunk*>& GetChunks()       { return m_Chunks;       }
		std::vector<Chunk*>& GetChunksToDraw() { return m_ChunksToDraw; }

		const glm::vec3& CalculateChunkAbsolutePosition(const glm::vec3& position);

	private:
		std::string filePath;
		bool m_QuitStatus = false;
		Player m_Player;
		std::vector<Chunk*> m_Chunks;
		std::vector<Chunk*> m_ChunksToDraw;

	private:
		void DeleteUnusedChunks(const glm::vec3& position);

	private:
		static World* s_Instance;

	};

}