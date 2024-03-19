#include "World.h"

#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/FrustumCulling.h"
#include "Renderer/Renderer.h"

#include "World/WorldGenerator.h"

#include "Core/Core.h"
#include "Core/Utils.h"

namespace KuchCraft {

	World* World::s_Instance = nullptr;

	World::World(const std::string& path)
		: filePath(path)
	{
		s_Instance = this;
		// Load data

		// Set data
		WorldGenerator::Init(4206999);
		m_Player.SetPosition({ 2000.0f, 75.0f, 2000.0f });

		// Loading stuff
		PreLoadWorld();
	}

	World::~World()
	{
		// Free memory
		Shutdown();
	}

	void World::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(KC_BIND_EVENT_FN(World::OnKeyPressed));

		m_Player.OnEvent(event);
	}

	bool World::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift   = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::F3: Renderer::FlipShowStatsStatus(); break;
		case Key::F1: ReloadAllChunks(); break;
		}

		return false;
	}

	void World::OnUpdate(float dt)
	{
		ClearStats();

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
		bool breakStatus = false;
		int totalChunksBuilded = 0;

		for (float x  = playerPosition.x - playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				   x <= playerPosition.x + playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				   x += chunk_size_XZ)
		{
			for (float z =  playerPosition.z - playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				       z <= playerPosition.z + playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				       z += chunk_size_XZ)
			{
				const int chunkIndex = GetChunkIndex({ x, 0.0f, z });

				if (chunkIndex >= 0 && chunkIndex < m_Chunks.size())
				{
					if (m_Chunks[chunkIndex] == nullptr)
					{
						m_Chunks[chunkIndex] = Chunk::Create({ x, 0.0f, z });
						m_WorldStats.ChunksInMemory++;
					}

					if (m_Chunks[chunkIndex]->NeedToBuild())
					{
						m_Chunks[chunkIndex]->Build();
						m_WorldStats.TotalBuiltChunks++;

						totalChunksBuilded++;
						if (totalChunksBuilded == max_chunks_to_Build)
							breakStatus = true;
					}

					m_ChunksToUpdate.push_back(m_Chunks[chunkIndex]);
					if (breakStatus)
						break;
				}
			}
			if (breakStatus)
				break;
		}
		m_WorldStats.ActiveChunks = static_cast<uint32_t>(m_ChunksToUpdate.size());

		// If needed recreate chunks
		constexpr int max_chunks_to_recreate = 1;
		int totalChunksRecreated = 0;

		for (const auto& c : m_ChunksToUpdate)
		{
			if (c->NeedToRecreate())
			{
				c->Recreate();
				totalChunksRecreated++;
				m_WorldStats.TotalRecreatedChunks++;
				if (totalChunksRecreated == max_chunks_to_recreate)
					break;
			}

		}

		// Find chunks to render
		m_ChunksToRender.clear();
		m_ChunksToRender.reserve(surroundingChunksCount / 2); // It is very likely that we will not render all the chunks around the player
		FrustumCulling::GetChunksToRender(m_ChunksToRender, m_ChunksToUpdate, m_Player.GetCamera());
		m_WorldStats.ChunksToRender = static_cast<uint32_t>(m_ChunksToRender.size());

		// Delete from memory chunks that are too far away
		DeleteUnusedChunks(playerPosition);
	}

	void World::Render()
	{
		Renderer::BeginWorld(m_Player.GetCamera());
		Renderer::SetWaterTintStatus(GetBlock(m_Player.GetEyePosition()) == BlockType::Water);

		Renderer::BeginChunk();
		for (const auto& c : m_ChunksToRender)
			Renderer::RenderChunk(c);
		Renderer::EndChunk();

		Renderer::BeginSkybox();
		Renderer::RenderSkybox();
		Renderer::EndSkybox();

		Renderer::BeginWater();
		for (const auto& c : m_ChunksToRender)
			Renderer::RenderWater(c);
		Renderer::EndWater();

		// Text to render
		if (Renderer::GetShowStatsStatus())
		{
			const auto& position = m_Player.GetPosition();
			const auto& rotation = m_Player.GetRotation();

			const auto& rendererStats = Renderer::GetStats();
			const auto& graphicalInfo = Renderer::GetGraphicalInfo();

			std::string text = "Player:"
				"\n    Position: " + VecToString(position) +
				"\n    Rotation: " + VecToString(glm::vec2(glm::degrees(rotation.x), glm::degrees(rotation.y))) +
				"\n    Chunk ID: " + std::to_string(GetChunkIndex(position)) +
				"\nWorld:"
				"\n    Seed: " + std::to_string(WorldGenerator::GetSeed()) +
				"\n    Chunks:"
				"\n      - active: "    + std::to_string(m_WorldStats.ActiveChunks        ) +
				"\n      - to render: " + std::to_string(m_WorldStats.ChunksToRender      ) +
				"\n      - in memory: " + std::to_string(m_WorldStats.ChunksInMemory      ) +
				"\n      - built: "     + std::to_string(m_WorldStats.TotalBuiltChunks    ) +
				"\n      - recreated: " + std::to_string(m_WorldStats.TotalRecreatedChunks) +
				"\nRenderer:"
				"\n    Draw cals: " + std::to_string(rendererStats.DrawCalls) +
				"\n    Quads: "     + std::to_string(rendererStats.Quads) +
				"\n    Render time: "         + std::to_string(rendererStats.RenderTimer.ElapsedMillis()) + "ms" +
				"\n      - chunks:      "     + std::to_string(rendererStats.ChunkTimer.ElapsedMillis()) + "ms" +
				"\n      - skybox:      "     + std::to_string(rendererStats.SkyboxTimer.ElapsedMillis()) + "ms" +
				"\n      - water:         "   + std::to_string(rendererStats.WaterTimer.ElapsedMillis())  + "ms" +
				"\n      - text:            " + std::to_string(rendererStats.TextTimer.ElapsedMillis())   + "ms";
							 

			Renderer::RenderTextTopLeft(text, { 6.0f, 25.0f });

			std::string text2 = graphicalInfo.Renderer + "\n" + graphicalInfo.Version;

			Renderer::RenderTextNorm(text2, { 0.5f, 0.975f });
		}

		Renderer::EndWorld();
	}

	void World::SetBlock(const glm::vec3& position, const Block& block)
	{
		Chunk* chunk = GetChunk(position);
		if (chunk)
		{
			const int x = static_cast<int>(std::fmod(position.x, chunk_size_XZ ));
			const int y = static_cast<int>(std::fmod(position.y, chunk_size_Y  ));
			const int z = static_cast<int>(std::fmod(position.z, chunk_size_XZ ));

			chunk->Block[x][y][z] = block;
			chunk->Recreate();
			m_WorldStats.TotalRecreatedChunks++;

			// If it is an edge block, recreate the corresponding chunk
			if (x == 0)
			{
				auto c = GetChunk({ position.x - chunk_size_XZ, position.y, position.z });
				if (c)
				{
					c->Recreate();
					m_WorldStats.TotalRecreatedChunks++;
				}
			}
			if (x == chunk_size_XZ - 1)
			{
				auto c = GetChunk({ position.x + chunk_size_XZ, position.y, position.z });
				if (c)
				{
					c->Recreate();
					m_WorldStats.TotalRecreatedChunks++;
				}
			}
			if (z == 0)
			{
				auto c = GetChunk({ position.x, position.y, position.z - chunk_size_XZ });
				if (c)
				{
					c->Recreate();
					m_WorldStats.TotalRecreatedChunks++;
				}
			}
			if (z == chunk_size_XZ - 1)
			{
				auto c = GetChunk({ position.x, position.y, position.z + chunk_size_XZ });
				if (c)
				{
					c->Recreate();
					m_WorldStats.TotalRecreatedChunks++;
				}
			}
		}
	}

	Block World::GetBlock(const glm::vec3& position)
	{
		Chunk* chunk = GetChunk(position);
		if (chunk)
		{
			const int x = static_cast<int>(std::fmod(position.x, chunk_size_XZ));
			const int y = static_cast<int>(std::fmod(position.y, chunk_size_Y));
			const int z = static_cast<int>(std::fmod(position.z, chunk_size_XZ));

			return chunk->Block[x][y][z];
		}
		return Block(BlockType::None);	
	}

	int World::GetChunkIndex(const glm::vec3& position)
	{
		if (position.x < 0 || position.z < 0)
			return -1;

		const int index_X = static_cast<int>(position.x / chunk_size_XZ);
		const int index_Z = static_cast<int>(position.z / chunk_size_XZ);

		return index_X + index_Z * world_chunk_size;
	}

	Chunk* World::GetChunk(const glm::vec3& position)
	{
		const int index = GetChunkIndex(position);

		if (index >= 0 && index < m_Chunks.size())
		{
			if (m_Chunks[index] == nullptr)
			{
				m_Chunks[index] = Chunk::Create(position);
				m_WorldStats.ChunksInMemory++;
			}

			if (m_Chunks[index]->NeedToBuild())
			{
				m_Chunks[index]->Build();
				m_WorldStats.TotalBuiltChunks++;
			}

			return m_Chunks[index];
		}

		return nullptr;
	}

	void World::ReloadAllChunks()
	{
		for (int i = 0; i < m_Chunks.size(); i++)
		{
			if (m_Chunks[i])
			{
				delete m_Chunks[i];
				m_Chunks[i] = nullptr;
			}
		}
		m_WorldStats.ChunksInMemory = 0;
	}

	void World::ReloadChunks()
	{
		const auto& position             = m_Player.GetPosition();
		const auto& renderDistance       = m_Player.GetGraphicalSettings().RenderDistance;
		const auto& inMemoryKeptDistance = m_Player.GetGraphicalSettings().ChunksKeptInMemoryDistance;

		const float position_min_x = position.x - (renderDistance + inMemoryKeptDistance) * chunk_size_XZ;
		const float position_max_x = position.x + (renderDistance + inMemoryKeptDistance) * chunk_size_XZ;
		const float position_min_z = position.z - (renderDistance + inMemoryKeptDistance) * chunk_size_XZ;
		const float position_max_z = position.z + (renderDistance + inMemoryKeptDistance) * chunk_size_XZ;

		for (int i = 0; i < m_Chunks.size(); i++)
		{
			if (m_Chunks[i])
			{
				auto& position = m_Chunks[i]->GetPosition();
				
				if (position.x < position_min_x || position.x > position_max_x || 
				    position.z < position_min_z || position.z > position_max_z)
				{
					delete m_Chunks[i];
					m_Chunks[i] = nullptr;
					m_WorldStats.ChunksInMemory--;
				}
			}
		}
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
				m_WorldStats.ChunksInMemory--;
			}
		}
		for (float z = distance_minus_z; z <= distance_plus_z; z += chunk_size_XZ)
		{
			int index = GetChunkIndex({ distance_plus_x, position.y, z });
			if (index >= 0 && index < m_Chunks.size() && m_Chunks[index])
			{
				delete m_Chunks[index];
				m_Chunks[index] = nullptr;
				m_WorldStats.ChunksInMemory--;
			}
		}
		for (float x = distance_minus_x; x <= distance_plus_x; x += chunk_size_XZ)
		{
			int index = GetChunkIndex({ x, position.y, distance_minus_z });
			if (index >= 0 && index < m_Chunks.size() && m_Chunks[index])
			{
				delete m_Chunks[index];
				m_Chunks[index] = nullptr;
				m_WorldStats.ChunksInMemory--;
			}
		}
		for (float x = distance_minus_x; x <= distance_plus_x; x += chunk_size_XZ)
		{
			int index = GetChunkIndex({ x, position.y, distance_plus_z });
			if (index >= 0 && index < m_Chunks.size() && m_Chunks[index])
			{
				delete m_Chunks[index];
				m_Chunks[index] = nullptr;
				m_WorldStats.ChunksInMemory--;
			}
		}
	}

	void World::ClearStats()
	{
		m_WorldStats.ActiveChunks   = 0;
		m_WorldStats.ChunksToRender = 0;
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
				m_WorldStats.ChunksInMemory--;
			}
		}
		m_ChunksToRender.clear();
		m_ChunksToUpdate.clear();
		s_Instance = nullptr;

		WorldGenerator::ShutDown();
	}

	void World::PreLoadWorld()
	{
		const auto& playerPosition = m_Player.GetPosition();
		const auto& playerGraphicalSettings = m_Player.GetGraphicalSettings();

		// Find surrounding chunks to update
		for (float x  = playerPosition.x - playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				   x <= playerPosition.x + playerGraphicalSettings.RenderDistance * chunk_size_XZ;
				   x += chunk_size_XZ)
		{
			for (float z  = playerPosition.z - playerGraphicalSettings.RenderDistance * chunk_size_XZ;
					   z <= playerPosition.z + playerGraphicalSettings.RenderDistance * chunk_size_XZ;
					   z += chunk_size_XZ)
			{
				const int chunkIndex = GetChunkIndex({ x, 0.0f, z });

				if (chunkIndex >= 0 && chunkIndex < m_Chunks.size())
				{
					if (m_Chunks[chunkIndex] == nullptr)
					{
						m_Chunks[chunkIndex] = Chunk::Create({ x, 0.0f, z });
						m_WorldStats.ChunksInMemory++;
					}

					if (m_Chunks[chunkIndex]->NeedToBuild())
					{
						m_Chunks[chunkIndex]->Build();
						m_WorldStats.TotalBuiltChunks++;
					}

					if (m_Chunks[chunkIndex]->NeedToRecreate())
					{
						m_Chunks[chunkIndex]->Recreate();
						m_WorldStats.TotalRecreatedChunks++;
					}
				}
			}
		}
	}

}