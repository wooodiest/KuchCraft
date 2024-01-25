#include "World.h"

#include <iostream>

#include "Renderer/Renderer.h"

namespace KuchCraft {

	World::World()
	{
		for (int x = 0; x < chunk_amount; x++)
		{
			for (int z = 0; z < chunk_amount; z++)
			{
				m_Chunks.emplace_back(new Chunk({x * 16.0f, 0.0f, z * 16.0f}));
			}
		}
	}

	World::~World()
	{
		for (auto& v : m_Chunks)
			delete v;
	}

	Chunk* World::GetChunk(const glm::vec3& position)
	{
		int position_X = static_cast<int>(position.x / 16.0f);
		int position_Z = static_cast<int>(position.z / 16.0f);

		if (position_X >= 0 && position_X < chunk_amount &&
			position_Z >= 0 && position_Z < chunk_amount)
			return m_Chunks[position_Z * chunk_amount + position_X];
		else
			return nullptr;
		
	}

	Chunk::Chunk(const glm::vec3& position)
		: m_Position(position)
	{
	}

	void Chunk::Render()
	{
		for (int x = 0; x < chunk_size_X; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_Z; z++)
				{
					// Always render border blocks
					if (x == 0 || y == 0 || z == 0 || x == chunk_size_X - 1 || y == chunk_size_Y - 1 || z == chunk_size_Z - 1)
					{
						if (m_Blocks[x][y][z].m_BlockType != BlockType::Air)
							Renderer::DrawCube(glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z), m_Blocks[x][y][z]);
						continue;
					}

					if (m_Blocks[x][y][z].m_BlockType == BlockType::Air)
						continue;

					// If blocks near current block are air render that
					// TODO: we do not have to check all blocks every time
					if (m_Blocks[x - 1][y][z].m_BlockType == BlockType::Air)
					{
						Renderer::DrawCube(glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z), m_Blocks[x][y][z]);
						continue;
					}
					if (m_Blocks[x + 1][y][z].m_BlockType == BlockType::Air)
					{
						Renderer::DrawCube(glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z), m_Blocks[x][y][z]);
						continue;
					}
					
					if (m_Blocks[x][y - 1][z].m_BlockType == BlockType::Air)
					{
						Renderer::DrawCube(glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z), m_Blocks[x][y][z]);
						continue;
					}
					if (m_Blocks[x][y + 1][z].m_BlockType == BlockType::Air)
					{
						Renderer::DrawCube(glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z), m_Blocks[x][y][z]);
						continue;
					}

					if (m_Blocks[x][y][z - 1].m_BlockType == BlockType::Air)
					{
						Renderer::DrawCube(glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z), m_Blocks[x][y][z]);
						continue;
					}
					if (m_Blocks[x][y][z + 1].m_BlockType == BlockType::Air)
					{
						Renderer::DrawCube(glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z), m_Blocks[x][y][z]);
						continue;
					}
				}
			}
		}
	}
}