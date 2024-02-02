#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "World/Block.h"
#include "Renderer/Renderer.h"

namespace KuchCraft {

	constexpr int chunk_size_XZ = 16;
	constexpr int chunk_size_Y = 128;

	class Chunk
	{
	public:
		Block Blocks[chunk_size_XZ][chunk_size_Y][chunk_size_XZ];

	public:
		Chunk(const glm::vec3& position);
		~Chunk();

		void Render();
		void Recreate();
		void Build();

		bool NeedToRecreate() const { return m_NeedToRecreate; }
		bool NeedToBuild()    const { return m_NeedToBuild;    }

		void SetRebuildStatus(bool status)  { m_NeedToBuild = status;    }
		void SetRecreateStatus(bool status) { m_NeedToRecreate = status; }

		const glm::vec3& GetPosition()                 const { return m_Position;         }

		const std::vector<BlockType>& GetTextureList() const { return m_DrawListTextures; }
		std::vector<Vertex>& GetDrawList()                   { return m_DrawList;         }

		const std::vector<BlockType>& GetTextureListTransparent() const { return m_DrawListTexturesTransparent; }
		std::vector<Vertex>& GetDrawListTransparent()                   { return m_DrawListTransparent;         }

	private:
		glm::vec3 m_Position;
		bool m_NeedToRecreate = true;
		bool m_NeedToBuild    = true;

		std::vector<Vertex>    m_DrawList;
		std::vector<BlockType> m_DrawListTextures;
		std::vector<Vertex>    m_DrawListTransparent;
		std::vector<BlockType> m_DrawListTexturesTransparent;

	private:
		void AddToDrawList(const glm::mat4& model, const Vertex vertices[quad_vertex_count], int x, int y, int z);
		void AddToDrawListTransparent(const glm::mat4& model, const Vertex vertices[quad_vertex_count], int x, int y, int z);

	};

}