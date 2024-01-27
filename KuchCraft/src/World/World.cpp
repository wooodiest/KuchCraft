#include "World.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Random.h"
#include "Core/Input.h"

namespace KuchCraft {

	uint32_t            World::s_RenderDistance = 10;
	uint32_t            World::s_MaxChunksToRecreatePerFrame = 3;
	std::vector<Chunk*> World::s_Chunks;
	std::vector<Chunk*> World::s_ChunksToDraw;

	void World::Init()
	{
		s_Chunks.resize(world_chunk_size * world_chunk_size);
	}

	void World::Shutdown()
	{
		for (int i = 0; i < s_Chunks.size(); i++)
		{
			if (s_Chunks[i])
			{
				delete s_Chunks[i];
				s_Chunks[i] = nullptr;
			}
		}
		s_ChunksToDraw.clear();
	}

	void World::OnUpdate(float dt, const glm::vec3& position)
	{
		if (Input::IsKeyPressed(KeyCode::R))
			Reload();

		// Find chunks to draw
		s_ChunksToDraw.clear();
		s_ChunksToDraw.reserve((2 * s_RenderDistance + 1) * (2 * s_RenderDistance + 1));

		for (float x = position.x - s_RenderDistance * chunk_size_XZ; x <= position.x + s_RenderDistance * chunk_size_XZ; x += chunk_size_XZ)
		{
			for (float z = position.z - s_RenderDistance * chunk_size_XZ; z <= position.z + s_RenderDistance * chunk_size_XZ; z += chunk_size_XZ)
			{
				auto chunk = World::GetChunk({ x, position.y, z });
				if (chunk)
					s_ChunksToDraw.push_back(chunk);
			}
		}
		// If needed build
		for (auto& c : s_ChunksToDraw)
		{
			if (c->NeedToBuild())
				c->Build();		
		}
		// If needed recreate
		int maxChunksToRecreate = s_MaxChunksToRecreatePerFrame;
		for (auto& c : s_ChunksToDraw)
		{
			if (c->NeedToRecreate()) // Recreate if distance{player -> chunk} is greater than something
			{		
				c->Recreate();
				maxChunksToRecreate--;
				if (maxChunksToRecreate == 0)
					break;
			}
		}
	}

	void World::Render()
	{
		for (auto& c : s_ChunksToDraw)
		{
			Renderer::DrawList(c->GetDrawList(), c->GetTextureList());
		}
	}

	void World::Reload()
	{
		for (int i = 0; i < s_Chunks.size(); i++)
		{
			if (s_Chunks[i])
			{
				delete s_Chunks[i];
				s_Chunks[i] = nullptr;
			}
		}
	}

	void World::ReloadChunk(const glm::vec3& position)
	{
		// If needed, OnUpdate will build and recreate it
		int index = GetChunkIndex(position);
		if (s_Chunks[index])
		{
			delete s_Chunks[index];
			s_Chunks[index] = nullptr;
		}
	}

	void World::RebuildChunk(const glm::vec3& position)
	{
		int index = GetChunkIndex(position);
		if (s_Chunks[index])
		{
			s_Chunks[index]->SetRebuildStatus(true);
		}
	}

	void World::RecreateChunk(const glm::vec3& position)
	{
		int index = GetChunkIndex(position);
		if (s_Chunks[index])
		{
			s_Chunks[index]->SetRecreateStatus(true);
		}
	}

	void World::SetBlock(const glm::vec3& position, const Block& block)
	{
		Chunk* chunk = GetChunk(position);
		if (chunk)
		{
			int x = std::fmod(position.x, chunk_size_XZ);
			int y = std::fmod(position.y, chunk_size_Y);
			int z = std::fmod(position.z, chunk_size_XZ);
			chunk->blocks[x][y][z] = block;
			chunk->Recreate();
		}
	}

	int World::GetChunkIndex(const glm::vec3& position)
	{
		if (position.x < 0 || position.z < 0)
			return -1.0f;

		int index_X = static_cast<int>(position.x / chunk_size_XZ);
		int index_Z = static_cast<int>(position.z / chunk_size_XZ);

		return index_X + index_Z * world_chunk_size;
	}

	Chunk* World::GetChunk(const glm::vec3& position)
	{
		int index = GetChunkIndex(position);

		if (index >= 0 && index < s_Chunks.size())
		{
			if (s_Chunks[index] == nullptr)
				s_Chunks[index] = new Chunk(World::CalculateChunkAbsolutePosition(position));
			
			if (s_Chunks[index]->NeedToBuild())
				s_Chunks[index]->Build();
			
			return s_Chunks[index];
		}
		
		return nullptr;
	}

	const glm::vec3& World::CalculateChunkAbsolutePosition(const glm::vec3& position)
	{
		return { position.x - std::fmod(position.x, chunk_size_XZ), 0.0f, position.z - std::fmod(position.z, chunk_size_XZ) };
	}

	Chunk::Chunk(const glm::vec3& position)
		: m_Position(position)
	{
	}

	Chunk::~Chunk()
	{
		m_DrawList.clear();
		m_DrawListTextures.clear();
	}

	void Chunk::Recreate()
	{
		m_DrawList.clear();
		m_DrawList.reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_XZ * cube_vertex_count);
		m_DrawListTextures.clear();
		m_DrawListTextures.reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_XZ * cube_vertex_count / 4); // quad has 4 vertices

		Chunk* leftChunk   = World::GetChunk({ m_Position.x - chunk_size_XZ, m_Position.y, m_Position.z });
		Chunk* rightChunk  = World::GetChunk({ m_Position.x + chunk_size_XZ, m_Position.y, m_Position.z });
		Chunk* frontChunk  = World::GetChunk({ m_Position.x, m_Position.y, m_Position.z + chunk_size_XZ });
		Chunk* behindChunk = World::GetChunk({ m_Position.x, m_Position.y, m_Position.z - chunk_size_XZ });

		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
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
						if (leftChunk && Block::IsTranspaent(leftChunk->blocks[chunk_size_XZ - 1][y][z]))
							AddToDrawList(transform, vertices_left, x, y, z);
					}
					else if (x == chunk_size_XZ - 1)
					{
						checkRight = false;
						if (rightChunk && Block::IsTranspaent(rightChunk->blocks[0][y][z]))
							AddToDrawList(transform, vertices_right, x, y, z);
					}
					if (z == 0)
					{
						checkBehind = false;
						if (behindChunk && Block::IsTranspaent(behindChunk->blocks[x][y][chunk_size_XZ - 1]))
							AddToDrawList(transform, vertices_behind, x, y, z);
					}
					else if (z == chunk_size_XZ - 1)
					{
						checkFront = false;
						if (frontChunk && Block::IsTranspaent(frontChunk->blocks[x][y][0]))
							AddToDrawList(transform, vertices_front, x, y, z);
					}
					// Rest of bloks
					if (checkBottom && Block::IsTranspaent(blocks[x][y - 1][z]))
						AddToDrawList(transform, vertices_bottom, x, y, z);
								
					if (checkTop    && Block::IsTranspaent(blocks[x][y + 1][z]))
						AddToDrawList(transform, vertices_top, x, y, z);
					
					if (checkFront  && Block::IsTranspaent(blocks[x][y][z + 1]))
						AddToDrawList(transform, vertices_front, x, y, z);
					
					if (checkRight  && Block::IsTranspaent(blocks[x + 1][y][z]))
						AddToDrawList(transform, vertices_right, x, y, z);
					
					if (checkBehind && Block::IsTranspaent(blocks[x][y][z - 1]))
						AddToDrawList(transform, vertices_behind, x, y, z);
					
					if (checkLeft   && Block::IsTranspaent(blocks[x - 1][y][z]))
						AddToDrawList(transform, vertices_left, x, y, z);
								
				}
			}
		}
		m_DrawList.shrink_to_fit();
		m_NeedToRecreate = false;
	}

	void Chunk::Build()
	{
		// Try read from file. If cannot build by yourself
		FillWithRandomBlocks();

		m_NeedToBuild    = false;
		m_NeedToRecreate = true;
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
		BlockType top = static_cast<BlockType>(Random::Int(1, count));
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
				{
					if (y == 0)
						blocks[x][y][z].blockType = BlockType::Bedrock;
					else
					{
						if (y == 60)
							blocks[x][y][z].blockType = top;
						else if (y < 60)
						{
							int number = Random::Int(1, 1000);
							if (number == 50)
								blocks[x][y][z].blockType = BlockType::Air;
							else
								blocks[x][y][z].blockType = static_cast<BlockType>(Random::Int(1, count));
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

		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
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

	bool Block::IsTranspaent(const Block& block)
	{
		switch (block.blockType)
		{
			case BlockType::Air : return true;
		}
		return false;
	}

}