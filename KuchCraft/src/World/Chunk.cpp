#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "World/World.h"
#include "World/WorldGenerator.h"

#include "Renderer/Renderer.h"

namespace KuchCraft {

	Chunk::Chunk(const glm::vec3& position)
		: Position(position)
	{
	}

	Chunk::~Chunk()
	{
	}

	void Chunk::Recreate()
	{
		m_DrawList.StartRecreating();

		Chunk* leftChunk   = World::Get().GetChunk({ Position.x - chunk_size_XZ, Position.y, Position.z                 });
		Chunk* rightChunk  = World::Get().GetChunk({ Position.x + chunk_size_XZ, Position.y, Position.z                 });
		Chunk* frontChunk  = World::Get().GetChunk({ Position.x                , Position.y, Position.z + chunk_size_XZ });
		Chunk* behindChunk = World::Get().GetChunk({ Position.x                , Position.y, Position.z - chunk_size_XZ });

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
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(Position.x + x, Position.y + y, Position.z + z));

					// Water has separate draw list
					// TODO: Add other translucent blocks
					if (Block[x][y][z] == BlockType::Water && y != chunk_size_Y - 1)
					{
						// if (Blocks[x][y + 1][z] == BlockType::Air)
							// AddToDrawListWater(transform, vertices_top);	
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
						if (leftChunk && Block::IsTranspaent(leftChunk->Block[chunk_size_XZ - 1][y][z]))
							m_DrawList.Add(transform, vertices_left, Block[x][y][z]);
					}
					else if (x == chunk_size_XZ - 1)
					{
						checkRight = false;
						if (rightChunk && Block::IsTranspaent(rightChunk->Block[0][y][z]))
							m_DrawList.Add(transform, vertices_right, Block[x][y][z]);
					}
					if (z == 0)
					{
						checkBehind = false;
						if (behindChunk && Block::IsTranspaent(behindChunk->Block[x][y][chunk_size_XZ - 1]))
							m_DrawList.Add(transform, vertices_behind, Block[x][y][z]);
					}
					else if (z == chunk_size_XZ - 1)
					{
						checkFront = false;
						if (frontChunk && Block::IsTranspaent(frontChunk->Block[x][y][0]))
							m_DrawList.Add(transform, vertices_front, Block[x][y][z]);
					}

					// Rest of bloks
					if (checkBottom && Block::IsTranspaent(Block[x][y - 1][z]))
						m_DrawList.Add(transform, vertices_bottom, Block[x][y][z]);

					if (checkTop    && Block::IsTranspaent(Block[x][y + 1][z]))
						m_DrawList.Add(transform, vertices_top, Block[x][y][z]);

					if (checkFront  && Block::IsTranspaent(Block[x][y][z + 1]))
						m_DrawList.Add(transform, vertices_front, Block[x][y][z]);

					if (checkRight  && Block::IsTranspaent(Block[x + 1][y][z]))
						m_DrawList.Add(transform, vertices_right, Block[x][y][z]);

					if (checkBehind && Block::IsTranspaent(Block[x][y][z - 1]))
						m_DrawList.Add(transform, vertices_behind, Block[x][y][z]);

					if (checkLeft   && Block::IsTranspaent(Block[x - 1][y][z]))
						m_DrawList.Add(transform, vertices_left, Block[x][y][z]);

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

}