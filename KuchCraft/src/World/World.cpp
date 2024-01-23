#include "World.h"

#include <iostream>

#include "Renderer/Renderer.h"

namespace KuchCraft {

	static const int max_chunk_size = 1;

	World::World()
	{
		for (int x = 0; x < max_chunk_size; x++)
		{
			for (int z = 0; z < max_chunk_size; z++)
			{
				m_Chunks.emplace_back(new Chunk({x * 16.0f, 0.0f, z * 16.0f}));
			}
		}
	}

	Chunk* World::GetChunk(const glm::vec3& position)
	{
		int position_X = static_cast<int>(position.x / 16.0f);
		int position_Z = static_cast<int>(position.z / 16.0f);

		if (position_X >= 0 && position_X < max_chunk_size &&
			position_Z >= 0 && position_Z < max_chunk_size)
			return m_Chunks[position_Z * max_chunk_size + position_X];
		else
			return nullptr;
		
	}

	Chunk::Chunk(const glm::vec3& position)
		: m_Position(position)
	{
	}

	void Chunk::Render()
	{
		for (int x = 0; x < 16; x++)
		{
			for (int y = 0; y < 16; y++) // Change to 128
			{
				for (int z = 0; z < 16; z++)
				{
					Renderer::Get().DrawCube(glm::vec3( m_Position.x + x, m_Position.y + y, m_Position.z + z ), m_Blocks[x][y][z]);
				}
			}
		}
	}
}