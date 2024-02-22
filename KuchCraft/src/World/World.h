#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "World/Chunk.h"
#include "Game/Player.h"

namespace KuchCraft {

	class World
	{
	public:	
		World(const std::string& path);
		~World();

		void PreLoadWorld();

		inline static World& Get() { return *s_Instance; }

		void OnUpdate(float dt);
		void Render();

		// Utils
		void  SetBlock(const glm::vec3& position, const Block& block);
		Block GetBlock(const glm::vec3& position);

		int    GetChunkIndex(const glm::vec3& position);
		Chunk* GetChunk(const glm::vec3& position);

		const Camera& GetCamera() const { return m_Player.GetCamera(); }
		bool GetQuitStatus()      const { return m_QuitStatus;         }

	private:
		// Managment
		std::string filePath;
		bool m_QuitStatus = false;

		// Data
		Player m_Player;

		// Chunk data
		std::vector<Chunk*> m_Chunks{ world_chunk_size * world_chunk_size, nullptr }; // Temporary
		std::vector<Chunk*> m_ChunksToUpdate;
		std::vector<Chunk*> m_ChunksToRender;

	private:
		void Shutdown();
		void DeleteUnusedChunks(const glm::vec3& position);

	private:
		static World* s_Instance;

	};

}