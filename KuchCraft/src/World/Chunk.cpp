#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>

#include "World/World.h"
#include "World/WorldGenerator.h"

namespace KuchCraft {

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
		m_DrawList.                   clear();
		m_DrawListTextures.           clear();
		m_DrawListTransparent.        clear();
		m_DrawListTexturesTransparent.clear();
		m_DrawList.                   reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_XZ * cube_vertex_count                    );
		m_DrawListTextures.           reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_XZ * cube_vertex_count / quad_vertex_count);
		m_DrawListTransparent.        reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_XZ * cube_vertex_count);
		m_DrawListTexturesTransparent.reserve(chunk_size_XZ * chunk_size_XZ * chunk_size_XZ * cube_vertex_count / quad_vertex_count);

		Chunk* leftChunk   = World::Get().GetChunk({ m_Position.x - chunk_size_XZ, m_Position.y, m_Position.z                 });
		Chunk* rightChunk  = World::Get().GetChunk({ m_Position.x + chunk_size_XZ, m_Position.y, m_Position.z                 });
		Chunk* frontChunk  = World::Get().GetChunk({ m_Position.x                , m_Position.y, m_Position.z + chunk_size_XZ });
		Chunk* behindChunk = World::Get().GetChunk({ m_Position.x                , m_Position.y, m_Position.z - chunk_size_XZ });

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

					if (Blocks[x][y][z] == BlockType::Water && y != chunk_size_Y - 1)
					{
						if (Blocks[x][y + 1][z] == BlockType::Air)
							AddToDrawListTransparent(transform, vertices_top, x, y, z);

						continue;
					}

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
		m_DrawListTextures.shrink_to_fit();
		m_DrawListTextures.shrink_to_fit();
		m_DrawListTexturesTransparent.shrink_to_fit();
		m_NeedToRecreate = false;
	}

	void Chunk::Build()
	{
		// Check in file
		bool needToGenerate = true; // World::LoadChunk(this);
		// Else
		if (needToGenerate)
			WorldGenerator::Generate(this);
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

	void Chunk::AddToDrawListTransparent(const glm::mat4& model, const Vertex vertices[quad_vertex_count], int x, int y, int z)
	{
		for (int i = 0; i < quad_vertex_count; i++)
		{
			m_DrawListTransparent.emplace_back(Vertex{
				glm::vec3(model * glm::vec4(vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z, 1.0f)),
				glm::vec2(vertices[i].TexCoord.x, vertices[i].TexCoord.y),
				0.0f });
		}
		m_DrawListTexturesTransparent.push_back(Blocks[x][y][z]);
	}

}