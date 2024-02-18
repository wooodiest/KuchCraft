#include "World.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/FrustumCulling.h"
#include "Renderer/Renderer.h"

#include "World/WorldGenerator.h"

namespace KuchCraft {

	World* World::s_Instance = nullptr;

	World::World(const std::string& path)
		: filePath(path)
	{
		s_Instance = this;
		// Load data

		// Set data
		WorldGenerator::Init(4206999);
	}

	World::~World()
	{
		// Free memory
		Shutdown();
	}

	void World::OnUpdate(float dt)
	{
		// Update necessary stuff
		m_Player.OnUpdate(dt);

		// Useful extracted variables
		const auto& playerPosition          = m_Player.GetPosition();
		const auto& playerGraphicalSettings = m_Player.GetGraphicalSettings();

		// Find surrounding chunks to update
		size_t surroundingChunksCount = (2 * playerGraphicalSettings.RenderDistance + 1) * (2 * playerGraphicalSettings.RenderDistance + 1);
		m_ChunksToUpdate.clear();
		m_ChunksToUpdate.reserve(surroundingChunksCount);
	
		constexpr int max_chunks_to_Build = 1; // This is not a strict value, chunk Recreate() can also build surrounding chunks
		int totalChunksBuilded = 0;

		for (float x  = playerPosition.x - playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				   x <= playerPosition.x + playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				   x += chunk_size_XZ)
		{
			for (float z =  playerPosition.z - playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				       z <= playerPosition.z + playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				       z += chunk_size_XZ)
			{
				int chunkIndex = GetChunkIndex({ x, 0.0f, z });

				if (chunkIndex >= 0 && chunkIndex < m_Chunks.size())
				{
					if (m_Chunks[chunkIndex] == nullptr)
						m_Chunks[chunkIndex] = Chunk::Create({ x, 0.0f, z });

					if (m_Chunks[chunkIndex]->NeedToBuild())
					{
						m_Chunks[chunkIndex]->Build();

						totalChunksBuilded++;
						if (totalChunksBuilded == max_chunks_to_Build)
							break;
					}

					m_ChunksToUpdate.push_back(m_Chunks[chunkIndex]);
				}
			}
			if (totalChunksBuilded == max_chunks_to_Build)
				break;
		}

		// If needed recreate chunks
		constexpr int max_chunks_to_recreate = 1;
		int totalChunksRecreated = 0;

		for (const auto& c : m_ChunksToUpdate)
		{
			if (c->NeedToRecreate())
			{
				c->Recreate();
				totalChunksRecreated++;
				if (totalChunksRecreated == max_chunks_to_recreate)
					break;
			}

		}

		// Find chunks to render
		m_ChunksToRender.clear();
		m_ChunksToRender.reserve(surroundingChunksCount / 2); // It is very likely that we will not render all the chunks around the player
		FrustumCulling::GetChunksToRender(m_ChunksToRender, m_ChunksToUpdate, m_Player.GetCamera());
		
		// Delete from memory chunks that are too far away
		DeleteUnusedChunks(playerPosition);
	}

	void World::Render()
	{
		Renderer::SetWaterTintStatus(GetBlock(m_Player.GetCamera().GetPosition()) == BlockType::Water);

		Renderer::BeginChunk();
		for (const auto& c : m_ChunksToRender)
			Renderer::RenderChunk(c);
		Renderer::EndChunk();
	}

	void World::SetBlock(const glm::vec3& position, const Block& block)
	{
		Chunk* chunk = GetChunk(position);
		if (chunk)
		{
			int x = static_cast<int>(std::fmod(position.x, chunk_size_XZ ));
			int y = static_cast<int>(std::fmod(position.y, chunk_size_Y  ));
			int z = static_cast<int>(std::fmod(position.z, chunk_size_XZ ));

			chunk->Block[x][y][z] = block;
			chunk->Recreate();

			// If it is an edge block, recreate the corresponding chunk
			if (x == 0)
			{
				auto c = GetChunk({ position.x - chunk_size_XZ, position.y, position.z });
				if(c)
					c->Recreate();
			}
			if (x == chunk_size_XZ - 1)
			{
				auto c = GetChunk({ position.x + chunk_size_XZ, position.y, position.z });
				if (c)
					c->Recreate();
			}
			if (z == 0)
			{
				auto c = GetChunk({ position.x, position.y, position.z - chunk_size_XZ });
				if (c)
					c->Recreate();
			}
			if (z == chunk_size_XZ - 1)
			{
				auto c = GetChunk({ position.x, position.y, position.z + chunk_size_XZ });
				if (c)
					c->Recreate();
			}
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

			return chunk->Block[x][y][z];
		}
		return Block(BlockType::None);	
	}

	int World::GetChunkIndex(const glm::vec3& position)
	{
		if (position.x < 0 || position.z < 0)
			return -1;

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
				m_Chunks[index] = Chunk::Create(position);

			if (m_Chunks[index]->NeedToBuild())
				m_Chunks[index]->Build();

			return m_Chunks[index];
		}

		return nullptr;
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
		m_ChunksToRender.clear();
		m_ChunksToUpdate.clear();
		s_Instance = nullptr;

		WorldGenerator::ShutDown();
	}

}