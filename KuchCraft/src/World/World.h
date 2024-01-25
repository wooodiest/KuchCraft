#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace KuchCraft {

	constexpr int chunk_amount = 8;
	constexpr int chunk_size_X = 16;
	constexpr int chunk_size_Y = 128;
	constexpr int chunk_size_Z = 16;

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

		Block m_Blocks[chunk_size_X][chunk_size_Y][chunk_size_Z];
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