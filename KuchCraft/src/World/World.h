#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace KuchCraft {

	enum class BlockType
	{
		Air = 0,/*Water,*/
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
		BlockType m_BlockType = BlockType::Air;
		//glm::vec3 m_Rotation{ 0.0f, 0.0f, 0.0f }; // TODO: Change to not use rotation matrix

	};

	class Chunk
	{
	public:
		Chunk(const glm::vec3& position);

		Block m_Blocks[16][128][16];
		glm::vec3 m_Position;
		void Render();

	};

	class World
	{
	public:
		World();
		~World();
		Chunk* GetChunk(const glm::vec3& position);

	private:
		std::vector<Chunk*> m_Chunks;
	};

}