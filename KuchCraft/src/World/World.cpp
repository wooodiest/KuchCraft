#include "World.h"

#include <iostream>

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
		WorldGenerator::Init(0);
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

		WorldGenerator::ShutDown();
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

}