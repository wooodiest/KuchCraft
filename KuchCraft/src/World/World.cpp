#include "World.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Random.h"

namespace KuchCraft {

	std::vector<Chunk*> World::m_Chunks;
	std::vector<Chunk*> World::m_ChunksToDraw;

	void World::Init()
	{
		m_Chunks.reserve(world_chunk_size * world_chunk_size);

		for (int z = 0; z < world_chunk_size; z++)
		{
			for (int x = 0; x < world_chunk_size; x++)
			{
				m_Chunks.emplace_back(new Chunk({ x * chunk_size_X, 0.0f, z * chunk_size_Z }));

				// temporary
				auto chunk = World::GetChunk({ x * chunk_size_X, 0.0f, z * chunk_size_Z });
				chunk->FillWithRandomBlocks();
				m_ChunksToDraw.push_back(chunk);
			}
		}
	}

	void World::Shutdown()
	{
		for (auto& v : m_Chunks)
			delete v;
	}

	void World::OnUpdate(float dt)
	{
		// if change in chunk, recrete (FUTURE: if block is border block recreate also chunk next to)
		for (auto& c : m_ChunksToDraw)
		{
			if (c->NeedToRecreate())
				c->Recreate();					
		}		
	}

	void World::Render(const glm::vec3& position, const glm::vec2& rotation, float viewAngle)
	{
		for (auto& c : m_ChunksToDraw)
		{
			Renderer::DrawList(c->GetDrawList(), c->GetTextureList());
		}
	}

	glm::vec2 World::GetChunkIndex(const glm::vec3& position)
	{
		int index_X = static_cast<int>(position.x / chunk_size_X);
		int index_Z = static_cast<int>(position.z / chunk_size_Z);

		return { index_X, index_Z };
	}

	Chunk* World::GetChunk(const glm::vec3& position)
	{
		glm::vec2 chunkIndex = GetChunkIndex(position);

		int index = chunkIndex.x + chunkIndex.y * world_chunk_size;

		if (index >= 0 && index < m_Chunks.size())
			return m_Chunks[index];

		return nullptr;
	}

	Chunk::Chunk(const glm::vec3& position)
		: m_Position(position)
	{
	}

	void Chunk::Recreate()
	{
		m_DrawList.clear();
		m_DrawList.reserve(chunk_size_X * chunk_size_Y * chunk_size_Z * cube_vertex_count);
		m_DrawListTextures.clear();
		m_DrawListTextures.reserve(chunk_size_X * chunk_size_Y * chunk_size_Z * cube_vertex_count / 4); // quad has 4 vertices

		Chunk* leftChunk   = World::GetChunk({ m_Position.x - chunk_size_X, m_Position.y, m_Position.z });
		Chunk* rightChunk  = World::GetChunk({ m_Position.x + chunk_size_X, m_Position.y, m_Position.z });
		Chunk* frontChunk  = World::GetChunk({ m_Position.x, m_Position.y, m_Position.z + chunk_size_Z });
		Chunk* behindChunk = World::GetChunk({ m_Position.x, m_Position.y, m_Position.z - chunk_size_Z });

		for (int x = 0; x < chunk_size_X; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_Z; z++)
				{
					if (blocks[x][y][z] == BlockType::Air)
						continue;

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z));
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
						if (leftChunk && leftChunk->blocks[chunk_size_X - 1][y][z] == BlockType::Air)
							AddToDrawList(transform, vertices_left, x, y, z);
					}
					else if (x == chunk_size_X - 1)
					{
						checkRight = false;
						if (rightChunk && rightChunk->blocks[0][y][z] == BlockType::Air)
							AddToDrawList(transform, vertices_right, x, y, z);
					}
					if (z == 0)
					{
						checkBehind = false;
						if (behindChunk && behindChunk->blocks[x][y][chunk_size_Z - 1] == BlockType::Air)
							AddToDrawList(transform, vertices_behind, x, y, z);
					}
					else if (z == chunk_size_Z - 1)
					{
						checkFront = false;
						if (frontChunk && frontChunk->blocks[x][y][0] == BlockType::Air)
							AddToDrawList(transform, vertices_front, x, y, z);
					}
					// Rest of bloks
					if (checkBottom && blocks[x][y - 1][z] == BlockType::Air) 
						AddToDrawList(transform, vertices_bottom, x, y, z);
								
					if (checkTop    && blocks[x][y + 1][z] == BlockType::Air)
						AddToDrawList(transform, vertices_top, x, y, z);
					
					if (checkFront  && blocks[x][y][z + 1] == BlockType::Air)
						AddToDrawList(transform, vertices_front, x, y, z);
					
					if (checkRight  && blocks[x + 1][y][z] == BlockType::Air)
						AddToDrawList(transform, vertices_right, x, y, z);
					
					if (checkBehind && blocks[x][y][z - 1] == BlockType::Air)
						AddToDrawList(transform, vertices_behind, x, y, z);
					
					if (checkLeft   && blocks[x - 1][y][z] == BlockType::Air)
						AddToDrawList(transform, vertices_left, x, y, z);
								
				}
			}
		}
		m_DrawList.shrink_to_fit();
		m_NeedToRecreate = false;
	}

	void Chunk::AddToDrawList(const glm::mat4& model, const Vertex vertices[4], int x, int y, int z)
	{
		for (int i = 0; i < 4; i++)
		{
			m_DrawList.emplace_back(Vertex{
				glm::vec3(model * glm::vec4(vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z, 1.0f)),
				glm::vec2(vertices[i].TexCoord.x, vertices[i].TexCoord.y),
				0.0f });
		}
		m_DrawListTextures.push_back(blocks[x][y][z]);
	}

	void Chunk::FillWithRandomBlocks()
	{
		int count = (int)BlockType::LastElement - 1;
		for (int x = 0; x < chunk_size_X; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_Z; z++)
				{
					if (y == 0)
						blocks[x][y][z].blockType = BlockType::Bedrock;
					else
					{
						if (y < 30)
						{
							blocks[x][y][z].blockType = static_cast<BlockType>(Random::Int(0, count));
						}
						else
							blocks[x][y][z].blockType = BlockType::Air;
					}
				}
			}
		}
	}

	void Chunk::FillWithOneBlock()
	{
		int count = (int)BlockType::LastElement - 1;
		int block = Random::Int(1, count);

		for (int x = 0; x < chunk_size_X; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_Z; z++)
				{
					if (y == 0)
						blocks[x][y][z].blockType = BlockType::Bedrock;
					else
					{
						if (y < 60)
						{
							if (Random::Int(0, 250) == 0)
								blocks[x][y][z].blockType = BlockType::Air;
							else
								blocks[x][y][z].blockType = static_cast<BlockType>(block);
						}
						else
							blocks[x][y][z].blockType = BlockType::Air;
					}
					
				}
			}
		}
	}

}