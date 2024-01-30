#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "World/Chunk.h"
#include "Game/Player.h"

namespace KuchCraft {

	constexpr int world_chunk_size = 1000; // square: world_chunk_size x world_chunk_size

	class World
	{
	public:	
		World(const std::string& path);
		~World();

		void Shutdown();
		inline static World& Get() { return *s_Instance; }

		void OnUpdate(float dt);
		void Render();

		void  SetBlock(const glm::vec3& position, const Block& block);
		Block GetBlock(const glm::vec3& position);

		const Camera& GetCamera() const { return m_Player.GetCamera(); }
		bool GetQuitStatus()      const { return m_QuitStatus;         }

		int    GetChunkIndex(const glm::vec3& position);
		Chunk* GetChunk(const glm::vec3& position);

		std::vector<Chunk*>& GetChunks()       { return m_Chunks;       }
		std::vector<Chunk*>& GetChunksToDraw() { return m_ChunksToDraw; }

		const glm::vec3& CalculateChunkAbsolutePosition(const glm::vec3& position);

	private:
		std::string filePath;
		bool m_QuitStatus = false;
		Player m_Player;
		std::vector<Chunk*> m_Chunks;
		std::vector<Chunk*> m_ChunksToDraw;

	private:
		void DeleteUnusedChunks(const glm::vec3& position);

	private:
		static World* s_Instance;

	};

}