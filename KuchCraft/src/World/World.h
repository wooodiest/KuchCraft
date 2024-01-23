#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace KuchCraft {

	enum class BlockType
	{
		Air = 0, Water,
		Grass
	};

	class Block
	{
	public:
		BlockType m_BlockType = BlockType::Grass; // Temporary
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
		Chunk* GetChunk(const glm::vec3& position);

	private:
		std::vector<Chunk*> m_Chunks;
	};

}