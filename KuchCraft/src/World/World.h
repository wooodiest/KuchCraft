#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "World/Chunk.h"
#include "Core/Events/Event.h"
#include "Core/Events/KeyEvent.h"
#include "Game/Player.h"

namespace KuchCraft {

	struct WorldStats
	{
		uint32_t ActiveChunks   = 0;
		uint32_t ChunksToRender = 0;

		uint32_t ChunksInMemory       = 0;
		uint32_t TotalBuiltChunks     = 0;
		uint32_t TotalRecreatedChunks = 0;
	};

	class World
	{
	public:	
		World(const std::string& path);
		~World();

		void PreLoadWorld();

		inline static World& Get() { return *s_Instance; }

		void OnEvent(Event& event);
		bool OnKeyPressed(KeyPressedEvent& e);

		void OnUpdate(float dt);
		void Render();

		// Utils
		void  SetBlock(const glm::vec3& position, const Block& block);
		Block GetBlock(const glm::vec3& position);

		int    GetChunkIndex(const glm::vec3& position);
		Chunk* GetChunk(const glm::vec3& position);

		Camera& GetCamera() { return m_Player.GetCamera(); }
		bool GetQuitStatus() const { return m_QuitStatus; }

		// TODO: Fix do not work properly
		void ReloadChunks();

	private:
		// Managment
		std::string filePath;
		bool m_QuitStatus = false;

		// Stats
		WorldStats m_WorldStats;
		void ClearStats();

		// Data
		Player m_Player;
		std::string m_DebugText;

		// Chunk data
		std::vector<Chunk*> m_Chunks{ world_chunk_size * world_chunk_size, nullptr }; // Temporary
		std::vector<Chunk*> m_ChunksToUpdate;
		std::vector<Chunk*> m_ChunksToRender;

	private:
		void Shutdown();
		void DeleteUnusedChunks(const glm::vec3& position);
		std::string& GetDebugText();

	private:
		static World* s_Instance;

	};

}