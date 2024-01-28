#include "World.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Random.h"
#include "Core/Input.h"
#include "WorldGenerator.h"

namespace KuchCraft {

	World* World::s_Instance = nullptr;

	World::World(const std::string& path)
		: filePath(path)
	{
		s_Instance = this;
		// Load data

		// Set data
		WorldGenerator::SetSeed(0);
		m_Chunks.resize(world_chunk_size * world_chunk_size);
	}

	World::~World()
	{
		// Free memory
		Shutdown();
	}

	void World::Shutdown()
	{
		// Save data


		// Free memory
		for (int i = 0; i < m_Chunks.size(); i++)
		{
			if (m_Chunks[i])
			{
				delete m_Chunks[i];
				m_Chunks[i] = nullptr;
			}
		}
		m_ChunksToDraw.clear();
		s_Instance = nullptr;
	}

	void World::OnUpdate(float dt)
	{
		// Update necessary stuff
		m_Player.OnUpdate(dt);

		// Useful extracted variables
		const auto& playerPosition          = m_Player.GetPosition();
		const auto& playerGraphicalSettings = m_Player.GetGraphicalSettings();

		// Find chunks to draw
		m_ChunksToDraw.clear();
		m_ChunksToDraw.reserve((2 * playerGraphicalSettings.RenderDistance + 1) * (2 * playerGraphicalSettings.RenderDistance + 1));

		for (float x  = playerPosition.x - playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				   x <= playerPosition.x + playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				   x += chunk_size_XZ)
		{
			for (float z =  playerPosition.z - playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				       z <= playerPosition.z + playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				       z += chunk_size_XZ)
			{
				Chunk* chunk = World::GetChunk({ x, playerPosition.y, z });
				if (chunk)
					m_ChunksToDraw.push_back(chunk);
			}
		}
		// If needed build chunks
		for (auto& c : m_ChunksToDraw)
		{
			if (c->NeedToBuild())
				c->Build();		
		}
		// If needed recreate chunks
		int maxChunksToRecreate = playerGraphicalSettings.MaxChunksToRecreatePerFrame;
		for (auto& c : m_ChunksToDraw)
		{
			if (c->NeedToRecreate()) 
			{		
				c->Recreate();
				maxChunksToRecreate--;
				if (maxChunksToRecreate == 0)
					break;
			}
		}
		// Delete from memory chunks that are too far away
		DeleteUnusedChunks(playerPosition);
	}

	void World::Render()
	{
		for (auto& c : m_ChunksToDraw)
		{
			Renderer::DrawList(c->GetDrawList(), c->GetTextureList());
		}
	}

	void World::SetBlock(const glm::vec3& position, const Block& block)
	{
		Chunk* chunk = GetChunk(position);
		if (chunk)
		{
			int x = static_cast<int>(std::fmod(position.x, chunk_size_XZ ));
			int y = static_cast<int>(std::fmod(position.y, chunk_size_Y  ));
			int z = static_cast<int>(std::fmod(position.z, chunk_size_XZ ));
			chunk->Blocks[x][y][z] = block;
			chunk->Recreate();
		}
	}

	Block World::GetBlock(const glm::vec3& position)
	{
		Chunk* chunk = GetChunk(position);
		if (chunk)
		{
			int x = static_cast<int>(std::fmod(position.x, chunk_size_XZ));
			int y = static_cast<int>(std::fmod(position.y, chunk_size_Y));
			int z = static_cast<int>(std::fmod(position.z, chunk_size_XZ));
			return chunk->Blocks[x][y][z];
		}
		return Block();	
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

		if (index >= 0 && index < m_Chunks.size())
		{
			if (m_Chunks[index] == nullptr)
				m_Chunks[index] = new Chunk(World::CalculateChunkAbsolutePosition(position));
			
			if (m_Chunks[index]->NeedToBuild())
				m_Chunks[index]->Build();
			
			return m_Chunks[index];
		}
		
		return nullptr;
	}

	Chunk* World::GetChunkToRecreate(const glm::vec3& position)
	{
		int index = GetChunkIndex(position);

		if (index >= 0 && index < m_Chunks.size() && m_Chunks[index])
			return m_Chunks[index];
		
		return nullptr;
	}

	const glm::vec3& World::CalculateChunkAbsolutePosition(const glm::vec3& position)
	{
		return { position.x - std::fmod(position.x, chunk_size_XZ), 0.0f, position.z - std::fmod(position.z, chunk_size_XZ) };
	}

	void World::DeleteUnusedChunks(const glm::vec3& position)
	{
		const auto& renderDistance       = m_Player.GetGraphicalSettings().RenderDistance;
		const auto& inMemoryKeptDistance = m_Player.GetGraphicalSettings().ChunksKeptInMemoryDistance;
		// Get rid of chunks whose position is equal to
		const float distance_minus_x = position.x - (renderDistance + inMemoryKeptDistance + 1) * chunk_size_XZ;
		const float distance_plus_x  = position.x + (renderDistance + inMemoryKeptDistance + 1) * chunk_size_XZ;
		const float distance_minus_z = position.z - (renderDistance + inMemoryKeptDistance + 1) * chunk_size_XZ;
		const float distance_plus_z  = position.z + (renderDistance + inMemoryKeptDistance + 1) * chunk_size_XZ;

		for (float z = distance_minus_z; z <= distance_plus_z; z += chunk_size_XZ)
		{
			int index = GetChunkIndex({ distance_minus_x, position.y, z });
			if (index >= 0 && index < m_Chunks.size() && m_Chunks[index])
			{
				delete m_Chunks[index];
				m_Chunks[index] = nullptr;
			}
		}
		for (float z = distance_minus_z; z <= distance_plus_z; z += chunk_size_XZ)
		{
			int index = GetChunkIndex({ distance_plus_x, position.y, z });
			if (index >= 0 && index < m_Chunks.size() && m_Chunks[index])
			{
				delete m_Chunks[index];
				m_Chunks[index] = nullptr;
			}
		}
		for (float x = distance_minus_x; x <= distance_plus_x; x += chunk_size_XZ)
		{
			int index = GetChunkIndex({ x, position.y, distance_minus_z });
			if (index >= 0 && index < m_Chunks.size() && m_Chunks[index])
			{
				delete m_Chunks[index];
				m_Chunks[index] = nullptr;
			}
		}
		for (float x = distance_minus_x; x <= distance_plus_x; x += chunk_size_XZ)
		{
			int index = GetChunkIndex({ x, position.y, distance_plus_z });
			if (index >= 0 && index < m_Chunks.size() && m_Chunks[index])
			{
				delete m_Chunks[index];
				m_Chunks[index] = nullptr;
			}
		}
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
		// Check what geometry should be rendered
		m_DrawList.        clear();
		m_DrawListTextures.clear();
		m_DrawList.        reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_XZ * cube_vertex_count                    );
		m_DrawListTextures.reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_XZ * cube_vertex_count / quad_vertex_count);

		Chunk* leftChunk   = World::Get().GetChunkToRecreate({ m_Position.x - chunk_size_XZ, m_Position.y, m_Position.z                 });
		Chunk* rightChunk  = World::Get().GetChunkToRecreate({ m_Position.x + chunk_size_XZ, m_Position.y, m_Position.z                 });
		Chunk* frontChunk  = World::Get().GetChunkToRecreate({ m_Position.x                , m_Position.y, m_Position.z + chunk_size_XZ });
		Chunk* behindChunk = World::Get().GetChunkToRecreate({ m_Position.x                , m_Position.y, m_Position.z - chunk_size_XZ });

		// Go through all the blocks and corresponding blocks of chunk next to it
		// If a block is not air, check if the blocks surrounding it are transparant
		// If they do add to draw list correct quad
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
				{
					if (Blocks[x][y][z] == BlockType::Air)
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
						if (leftChunk && Block::IsTranspaent(leftChunk->Blocks[chunk_size_XZ - 1][y][z]))
							AddToDrawList(transform, vertices_left, x, y, z);
					}
					else if (x == chunk_size_XZ - 1)
					{
						checkRight = false;
						if (rightChunk && Block::IsTranspaent(rightChunk->Blocks[0][y][z]))
							AddToDrawList(transform, vertices_right, x, y, z);
					}
					if (z == 0)
					{
						checkBehind = false;
						if (behindChunk && Block::IsTranspaent(behindChunk->Blocks[x][y][chunk_size_XZ - 1]))
							AddToDrawList(transform, vertices_behind, x, y, z);
					}
					else if (z == chunk_size_XZ - 1)
					{
						checkFront = false;
						if (frontChunk && Block::IsTranspaent(frontChunk->Blocks[x][y][0]))
							AddToDrawList(transform, vertices_front, x, y, z);
					}
					// Rest of bloks
					if (checkBottom && Block::IsTranspaent(Blocks[x][y - 1][z]))
						AddToDrawList(transform, vertices_bottom, x, y, z);
								
					if (checkTop    && Block::IsTranspaent(Blocks[x][y + 1][z]))
						AddToDrawList(transform, vertices_top, x, y, z);
					
					if (checkFront  && Block::IsTranspaent(Blocks[x][y][z + 1]))
						AddToDrawList(transform, vertices_front, x, y, z);
					
					if (checkRight  && Block::IsTranspaent(Blocks[x + 1][y][z]))
						AddToDrawList(transform, vertices_right, x, y, z);
					
					if (checkBehind && Block::IsTranspaent(Blocks[x][y][z - 1]))
						AddToDrawList(transform, vertices_behind, x, y, z);
					
					if (checkLeft   && Block::IsTranspaent(Blocks[x - 1][y][z]))
						AddToDrawList(transform, vertices_left, x, y, z);
								
				}
			}
		}
		m_DrawList.shrink_to_fit();
		m_NeedToRecreate = false;
	}

	void Chunk::Build()
	{
		// Check in file
		bool needToGenerate = true; // World::LoadChunk(this);
		// Else
		if(needToGenerate)
			WorldGenerator::FillWithRandomBlocks(this);
			// Save to file

		m_NeedToBuild    = false;
		m_NeedToRecreate = true;
	}

	void Chunk::AddToDrawList(const glm::mat4& model, const Vertex vertices[quad_vertex_count], int x, int y, int z)
	{
		for (int i = 0; i < quad_vertex_count; i++)
		{
			m_DrawList.emplace_back(Vertex{
				glm::vec3(model * glm::vec4(vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z, 1.0f)),
				glm::vec2(vertices[i].TexCoord.x, vertices[i].TexCoord.y),
				0.0f });
		}
		m_DrawListTextures.push_back(Blocks[x][y][z]);
	}

	Block::Block(const BlockType& type)
		: blockType(type)
	{
	}

	bool Block::IsTranspaent(const Block& block)
	{
		switch (block.blockType)
		{
			case BlockType::Air : return true;
			case BlockType::None: return true;
		}
		return false;
	}

}