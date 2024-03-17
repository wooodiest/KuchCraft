#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "KuchCraftData.h"
#include "World/Block.h"
#include "Renderer/ChunkData.h"

namespace KuchCraft {

	class Chunk
	{
	public:
		Block Block[chunk_size_XZ][chunk_size_Y][chunk_size_XZ];

	public:
		Chunk(const glm::vec3& position);
		~Chunk();

		// Check what geometry should be rendered and builds draw list
		void Recreate();

		// Create blocks that make up the chunk
		void Build();

		// Build/Recreate utils
		bool NeedToRecreate() const { return m_NeedToRecreate; }
		bool NeedToBuild()    const { return m_NeedToBuild;    }

		void SetRebuildStatus(bool status)  { m_NeedToBuild = status;    }
		void SetRecreateStatus(bool status) { m_NeedToRecreate = status; }

		// Utils
		const glm::vec3&     GetPosition()  const { return m_Position; }
		const ChunkDrawList& GetDrawList()  const { return m_DrawList; } 

		static glm::vec3 CalculatePosition(const glm::vec3& position);
		static Chunk*    Create(const glm::vec3& position);

	private:
		// Chunk position starts at (0, 0, 0) and continue along the axis + (chunk_size_XZ, 0, chunk_size_XZ)
		const glm::vec3 m_Position;

		bool m_NeedToRecreate = true;
		bool m_NeedToBuild    = true;

		ChunkDrawList m_DrawList; 

	};

}