#include "kcpch.h"
#include "World/World.h"

#include "KuchCraft.h"

#include "Renderer/Renderer.h"
#include "Renderer/Renderer3D/Renderer3D.h"
#include "Renderer/Renderer2D/Renderer2D.h"
#include "Renderer/AssetManager.h"

#include "World/WorldGenerator.h"

#include "Core/Core.h"
#include "Core/Utils.h"

#include "Core/Application.h"

#include "World/Item.h"

namespace KuchCraft {

	World* World::s_Instance = nullptr;

	World::World(const std::string& path)
		: filePath(path)
	{
		s_Instance = this;
		// Load data

		// Set data
		WorldGenerator::Init(4206999);
		m_Player = Player({ 2019.0f, 69.0f, 1993.0f }, { glm::radians(180.0f), 0.0f}, GameMode::Creative);

		// Loading stuff
		PreLoadWorld();
	}

	World::~World()
	{
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
			
		}

		return false;
	}

	void World::OnUpdate(float dt)
	{
		ClearStats();

		// Update necessary stuff
		m_Player.OnUpdate(dt);

		// Useful extracted variables
		const auto& playerPosition    = m_Player.GetPosition();
		const auto& graphicalSettings = KuchCraft::GetSettings();

		// Find surrounding chunks to update
		size_t surroundingChunksCount = (2 * graphicalSettings.RenderDistance + 1) * (2 * graphicalSettings.RenderDistance + 1);
		m_ChunksToUpdate.clear();
		m_ChunksToUpdate.reserve(surroundingChunksCount);

		constexpr int max_chunks_to_Build = 1; // This is not a strict value, chunk Recreate() can also build surrounding chunks
		bool breakStatus = false;
		int totalChunksBuilded = 0;

		for (float x  = playerPosition.x - graphicalSettings.RenderDistance * chunk_size_XZ;
				   x <= playerPosition.x + graphicalSettings.RenderDistance * chunk_size_XZ;
				   x += chunk_size_XZ)
		{
			for (float z =  playerPosition.z - graphicalSettings.RenderDistance * chunk_size_XZ;
				       z <= playerPosition.z + graphicalSettings.RenderDistance * chunk_size_XZ;
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
		
		// Delete from memory chunks that are too far away
		DeleteUnusedChunks(playerPosition);

		// Renderer draw commands
		Renderer3D::DrawChunks(m_ChunksToUpdate);

		if (GetItem(m_Player.GetEyePosition()).Type == ItemType::Water)
			Renderer3D::DrawWaterTinted();

		if (m_Player.GetTargetedBlockStatus())
			Renderer3D::DrawOutlinedBlock(m_Player.GetTargetedItem().Position);

		Renderer2D::DrawTextTopLeft(m_Player.GetDebugText() + GetDebugText() + Renderer::GetDubugText(), { 5.0f, 5.0f });

		// Tmp, example
		{
			// Cubes/ lines
			Renderer3D::DrawItem({ 2000.0f, 62.0f, 2000.0f }, Item(ItemType::None, ItemRotation::Deg90));
			
			Renderer3D::DrawCube({ 2000.0f, 70.0f, 2000.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
			Renderer3D::DrawCube({ 2000.0f, 80.0f, 2000.0f }, { 0.0f ,0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, AssetManager::GetHeartFullTexture());
				
			Renderer3D::DrawLine({ 2000.0f, 65.0f, 2000.0f }, { 1940.0f, 70.0f, 1970.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
			
			// 3D quads
			static glm::vec3 quadRotation{ 0.0f, 0.0f, 0.0f };
			quadRotation.y += glm::radians(45.0f) * dt;
			Renderer3D::DrawQuad({ 2000.0f, 65.0f, 2000.0f }, quadRotation, { 2.0f, 0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f });
			
			static glm::vec3 texturedQuadRotation{ 0.0f, 0.0f, 0.0f };
			texturedQuadRotation.y -= glm::radians(45.0f) * dt;
			texturedQuadRotation.x += glm::radians(30.0f) * dt;
			Renderer3D::DrawQuad({ 1995.0f, 65.0f, 1990.0f }, texturedQuadRotation, { 2.0f, 1.0f }, AssetManager::GetItemTexture(ItemType::DiamondOre));
			
			static glm::vec3 fontRotation{ 0.0f, glm::radians(90.0f), 0.0f};
			fontRotation.y += glm::radians(10.0f) * dt;
			Renderer3D::DrawText("JD2000", { 1940.0f, 70.0f, 1970.0f }, fontRotation, { 1.0f, 0.0f, 1.0f, 1.0 }, 3.0f);
			
			// TODO: additional random position and angle dissplacement
			Renderer3D::DrawQuad({ 2016.5f, 69.5f, 1993.5f }, { 0.0f, glm::radians(45.0f        ), 0.0f }, { 0.5f, 0.5f }, AssetManager::GetItemTexture(ItemType::RedTulipan));
			Renderer3D::DrawQuad({ 2016.45f, 69.5f, 1993.5f }, { 0.0f, glm::radians(45.0f + 90.0f), 0.0f }, { 0.5f, 0.5f }, AssetManager::GetItemTexture(ItemType::RedTulipan));

			// Rendere2D: some ui
			auto& [width, height] = Application::Get().GetWindow().GetWindowSize();

			Renderer2D::DrawQuad({ 150.0f, height - 150.0f, 0.0f }, { 150.0f, 150.0f }, { 1.0f, 1.0f, 1.0f, 0.5f });

			glm::vec3 hotbarPosition{ 0.5f * width, 33.0f, 1.0f };
			Renderer2D::DrawQuad(hotbarPosition, { 273.0f, 33.0f }, AssetManager::GetHotbarTexture());

			glm::vec3 hotbarSelectionPosition{ 0.5f * width - 3 * 60.0f, 34.0f, 1.0f };
			Renderer2D::DrawQuad(hotbarSelectionPosition, { 33.0f, 33.0f }, AssetManager::GetHotbarSelectionTexture());

			Renderer2D::DrawQuadN({ 0.5f, 0.5f, 1.0f }, { 15.0f, 15.0f }, AssetManager::GetCrosshairTexture());

			// Hearts
			glm::vec3 heartInitialPosition{ 0.5f * width - 273.0f, 80.0f, 1.0f };

			for (float i = 0; i < 10; i++)
			{
				if (i < 6)
					Renderer2D::DrawQuad({ heartInitialPosition.x + i * 2 * 11.0f + 13.0f, heartInitialPosition.y , 1.0f }, { 13.0f, 13.0f }, AssetManager::GetHeartFullTexture());
				else
					Renderer2D::DrawQuad({ heartInitialPosition.x + i * 2 * 11.0f + 13.0f, heartInitialPosition.y , 1.0f }, { 10.0f, 10.0f }, AssetManager::GetHeartEmptyTexture());
			}

			// Armor
			glm::vec3 armorInitialPosition{ 0.5f * width - 273.0f, 105.0f, 1.0f };
			for (float i = 0; i < 10; i++)
			{
				if (i < 3)
					Renderer2D::DrawQuad({ armorInitialPosition.x + i * 2 * 11.0f + 13.0f, armorInitialPosition.y , 1.0f }, { 10.0f, 10.0f }, AssetManager::GetArmorFullTexture());
				else
					Renderer2D::DrawQuad({ armorInitialPosition.x + i * 2 * 11.0f + 13.0f, armorInitialPosition.y , 1.0f }, { 10.0f, 10.0f }, AssetManager::GetArmorEmptyTexture());
			}

			// Food
			glm::vec3 foodInitialPosition{ 0.5f * width + 273.0f, 80.0f, 1.0f };
			for (float i = 0; i < 10; i++)
			{
				if (i < 6)
					Renderer2D::DrawQuad({ foodInitialPosition.x - i * 2 * 11.0f - 13.0f, foodInitialPosition.y , 1.0f }, { 10.0f, 10.0f }, AssetManager::GetFoodEmptyTexture());
				else
					Renderer2D::DrawQuad({ foodInitialPosition.x - i * 2 * 11.0f - 13.0f, foodInitialPosition.y , 1.0f }, { 13.0f, 13.0f }, AssetManager::GetFoodFullTexture());
			}
		}
	}

	void World::SetItem(const glm::vec3& position, const Item& block)
	{
		Chunk* chunk = GetChunk(position);
		if (chunk)
		{
			const int x = static_cast<int>(std::fmod(position.x, chunk_size_XZ ));
			const int y = static_cast<int>(std::fmod(position.y, chunk_size_Y  ));
			const int z = static_cast<int>(std::fmod(position.z, chunk_size_XZ ));

			chunk->Items[x][y][z] = block;
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
			else if (x == chunk_size_XZ - 1)
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
			else if (z == chunk_size_XZ - 1)
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

	Item World::GetItem(const glm::vec3& position)
	{
		Chunk* chunk = GetChunk(position);
		if (chunk)
		{
			if (position.y < 0.0f || position.y >= chunk_size_Y)
				return Item(ItemType::Air);

			const int x = static_cast<int>(std::fmod(position.x, chunk_size_XZ));
			const int y = static_cast<int>(std::fmod(position.y, chunk_size_Y));
			const int z = static_cast<int>(std::fmod(position.z, chunk_size_XZ));

			return chunk->Items[x][y][z];
		}
		return Item(ItemType::Air);
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

	void World::ReloadChunks()
	{
		//TODO: Fix
		const auto& position          = m_Player.GetPosition();
		const auto& graphicalSettings = KuchCraft::GetSettings();

		const float position_min_x = position.x - (graphicalSettings.RenderDistance + graphicalSettings.ChunksKeptInMemoryDistance) * chunk_size_XZ;
		const float position_max_x = position.x + (graphicalSettings.RenderDistance + graphicalSettings.ChunksKeptInMemoryDistance) * chunk_size_XZ;
		const float position_min_z = position.z - (graphicalSettings.RenderDistance + graphicalSettings.ChunksKeptInMemoryDistance) * chunk_size_XZ;
		const float position_max_z = position.z + (graphicalSettings.RenderDistance + graphicalSettings.ChunksKeptInMemoryDistance) * chunk_size_XZ;

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
		const auto& graphicalSettings = KuchCraft::GetSettings();

		// Get rid of chunks whose position is equal to
		const float distance_plus_x  = position.x + (graphicalSettings.RenderDistance + graphicalSettings.ChunksKeptInMemoryDistance + 1) * chunk_size_XZ;
		const float distance_minus_z = position.z - (graphicalSettings.RenderDistance + graphicalSettings.ChunksKeptInMemoryDistance + 1) * chunk_size_XZ;
		const float distance_minus_x = position.x - (graphicalSettings.RenderDistance + graphicalSettings.ChunksKeptInMemoryDistance + 1) * chunk_size_XZ;
		const float distance_plus_z  = position.z + (graphicalSettings.RenderDistance + graphicalSettings.ChunksKeptInMemoryDistance + 1) * chunk_size_XZ;

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

	std::string& World::GetDebugText()
	{
		m_DebugText =
			"\nWorld:"
			"\n    Seed: " + std::to_string(WorldGenerator::GetSeed()) +
			"\n    Chunk ID: " + std::to_string(GetChunkIndex(m_Player.GetPosition())) +
			"\n    Chunks:"
			"\n      - active: " + std::to_string(m_WorldStats.ActiveChunks) +
			"\n      - to render: " + std::to_string(Renderer::GetStatistics().ChunksToRender) +
			"\n      - in memory: " + std::to_string(m_WorldStats.ChunksInMemory) +
			"\n      - built: " + std::to_string(m_WorldStats.TotalBuiltChunks) +
			"\n      - recreated: " + std::to_string(m_WorldStats.TotalRecreatedChunks);

		return m_DebugText;
	}

	void World::ClearStats()
	{
		m_WorldStats.ActiveChunks   = 0;
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
		m_ChunksToUpdate.clear();
		s_Instance = nullptr;

		WorldGenerator::ShutDown();
	}

	void World::PreLoadWorld()
	{
		const auto& playerPosition    = m_Player.GetPosition();
		const auto& graphicalSettings = KuchCraft::GetSettings();

		// Find surrounding chunks to update
		for (float x  = playerPosition.x - graphicalSettings.RenderDistance * chunk_size_XZ;
				   x <= playerPosition.x + graphicalSettings.RenderDistance * chunk_size_XZ;
				   x += chunk_size_XZ)
		{
			for (float z  = playerPosition.z - graphicalSettings.RenderDistance * chunk_size_XZ;
					   z <= playerPosition.z + graphicalSettings.RenderDistance * chunk_size_XZ;
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