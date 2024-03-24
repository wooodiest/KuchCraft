#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "World/World.h"
#include "World/WorldGenerator.h"

namespace KuchCraft {

	Chunk::Chunk(const glm::vec3& position)
		: m_Position(position)
	{
	}

	Chunk::~Chunk()
	{
	}

	void Chunk::Recreate()
	{
		m_DrawList.StartRecreating();

		Chunk* leftChunk   = World::Get().GetChunk({ m_Position.x - chunk_size_XZ, m_Position.y, m_Position.z                 });
		Chunk* rightChunk  = World::Get().GetChunk({ m_Position.x + chunk_size_XZ, m_Position.y, m_Position.z                 });
		Chunk* frontChunk  = World::Get().GetChunk({ m_Position.x                , m_Position.y, m_Position.z + chunk_size_XZ });
		Chunk* behindChunk = World::Get().GetChunk({ m_Position.x                , m_Position.y, m_Position.z - chunk_size_XZ });

		// Go through all the blocks and corresponding blocks of chunk next to it
		// If a block is not air, check if the blocks surrounding it are transparant
		// If they do add to draw list correct quad
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
				{
					// Skip
					if (Block[x][y][z] == BlockType::Air)
						continue;

					// Calculate model matrix
					// TODO: Rotation
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z));

					// Water has separate draw list
					if (Block[x][y][z] == BlockType::Water && y != chunk_size_Y - 1)
					{
						 if (Block[x][y + 1][z] == BlockType::Air)
							 m_DrawList.AddWater(transform, vertices_top);
						continue;
					}

					// We don't always have to, or even it's dangerous, to check all surrounding blocks
					bool checkBottom = true, checkTop    = true;
					bool checkFront  = true, checkBehind = true;
					bool checkRight  = true, checkLeft   = true;

					// Bedrock only visible from up
					if (y == 0)
					{
						checkBottom = false;
						checkFront  = false; checkBehind = false;
						checkRight  = false; checkLeft   = false;
					}
					// Maximum y block
					else if (y == chunk_size_Y - 1)
						checkTop = false;

					// Chunk border bloks
					if (x == 0)
					{
						checkLeft = false;
						if (leftChunk && Block::IsTransparent(leftChunk->Block[chunk_size_XZ - 1][y][z]))
							m_DrawList.Add(glm::ivec3(x, y, z), vertices_left_index, Block[x][y][z]);
					}
					else if (x == chunk_size_XZ - 1)
					{
						checkRight = false;
						if (rightChunk && Block::IsTransparent(rightChunk->Block[0][y][z]))
							m_DrawList.Add(glm::ivec3(x, y, z), vertices_right_index, Block[x][y][z]);
					}
					if (z == 0)
					{
						checkBehind = false;
						if (behindChunk && Block::IsTransparent(behindChunk->Block[x][y][chunk_size_XZ - 1]))
							m_DrawList.Add(glm::ivec3(x, y, z), vertices_behind_index, Block[x][y][z]);
					}
					else if (z == chunk_size_XZ - 1)
					{
						checkFront = false;
						if (frontChunk && Block::IsTransparent(frontChunk->Block[x][y][0]))
							m_DrawList.Add(glm::ivec3(x, y, z), vertices_front_index, Block[x][y][z]);
					}

					// Rest of bloks
					if (checkBottom && Block::IsTransparent(Block[x][y - 1][z]))
						m_DrawList.Add(glm::ivec3(x, y, z), vertices_bottom_index, Block[x][y][z]);

					if (checkTop    && Block::IsTransparent(Block[x][y + 1][z]))
						m_DrawList.Add(glm::ivec3(x, y, z), vertices_top_index, Block[x][y][z]);

					if (checkFront  && Block::IsTransparent(Block[x][y][z + 1]))
						m_DrawList.Add(glm::ivec3(x, y, z), vertices_front_index, Block[x][y][z]);

					if (checkRight  && Block::IsTransparent(Block[x + 1][y][z]))
						m_DrawList.Add(glm::ivec3(x, y, z), vertices_right_index, Block[x][y][z]);

					if (checkBehind && Block::IsTransparent(Block[x][y][z - 1]))
						m_DrawList.Add(glm::ivec3(x, y, z), vertices_behind_index, Block[x][y][z]);

					if (checkLeft   && Block::IsTransparent(Block[x - 1][y][z]))
						m_DrawList.Add(glm::ivec3(x, y, z), vertices_left_index, Block[x][y][z]);

				}
			}
		}				              
		m_NeedToRecreate = false;
		m_DrawList.EndRecreating();
	}

	void Chunk::Build()
	{
		// Check in file
		bool needToGenerate = true; // World::LoadChunk(this);
		// Else
		if (needToGenerate)
			WorldGenerator::Generate(this);
		// Save to file

		m_NeedToBuild    = false;
		m_NeedToRecreate = true;
	}

	glm::vec3 Chunk::CalculatePosition(const glm::vec3& position)
	{
		return { position.x - std::fmod(position.x, chunk_size_XZ), 0.0f, position.z - std::fmod(position.z, chunk_size_XZ) };
	}

	Chunk* Chunk::Create(const glm::vec3& position)
	{
		return new Chunk(CalculatePosition(position));
	}

}