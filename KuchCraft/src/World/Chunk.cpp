#include "kcpch.h"
#include "World/Chunk.h"

#include "World/World.h"
#include "World/WorldGenerator.h"

namespace KuchCraft {

	Chunk::Chunk(const glm::vec3& position)
		: m_Position(position)
	{
	}

	Chunk::~Chunk()
	{
	}

	void Chunk::Recreate()
	{
		m_DrawList.StartRecreating();

		Chunk* leftChunk   = World::Get().GetChunk({ m_Position.x - chunk_size_XZ, m_Position.y, m_Position.z                 });
		Chunk* rightChunk  = World::Get().GetChunk({ m_Position.x + chunk_size_XZ, m_Position.y, m_Position.z                 });
		Chunk* frontChunk  = World::Get().GetChunk({ m_Position.x                , m_Position.y, m_Position.z + chunk_size_XZ });
		Chunk* behindChunk = World::Get().GetChunk({ m_Position.x                , m_Position.y, m_Position.z - chunk_size_XZ });

		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
				{
					if (Items[x][y][z].Type == ItemType::Air)
						continue;

					if (Items[x][y][z].IsFoliageQuad())
					{
						m_DrawList.AddFoliageQuad({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, Items[x][y][z]);
						continue;
					}

					if (Items[x][y][z].IsTransparentBlock())
					{
						// tmp
						if (Items[x][y][z].Type == ItemType::Water && y != chunk_size_Y - 1 && Items[x][y + 1][z].Type == ItemType::Air)
							m_DrawList.AddWater(glm::translate(glm::mat4(1.0f), glm::vec3(m_Position.x + x, m_Position.y + y, m_Position.z + z)), vertices_top);

						continue;
					}

					// Solid blocks
					// We don't always have to, or even it's dangerous, to check all surrounding blocks
					bool checkBottom = true, checkTop    = true;
					bool checkFront  = true, checkBehind = true;
					bool checkRight  = true, checkLeft   = true;

					// check edge cases
					if (y == 0)
						checkBottom = false;
					else if (y == chunk_size_Y - 1)
						checkTop = false;

					if (x == 0)
					{
						checkLeft = false;
						if (leftChunk && leftChunk->Items[chunk_size_XZ - 1][y][z].IsTranslucent())
							m_DrawList.AddSolid({ x, y, z }, vertices_left_index, Items[x][y][z]);
					}
					else if (x == chunk_size_XZ - 1)
					{
						checkRight = false;
						if (rightChunk && rightChunk->Items[0][y][z].IsTranslucent())
							m_DrawList.AddSolid({ x, y, z }, vertices_right_index, Items[x][y][z]);
					}

					if (z == 0)
					{
						checkBehind = false;
						if (behindChunk && behindChunk->Items[x][y][chunk_size_XZ - 1].IsTranslucent())
							m_DrawList.AddSolid({ x, y, z }, vertices_behind_index, Items[x][y][z]);
					}
					else if (z == chunk_size_XZ - 1)
					{
						checkFront = false;
						if (frontChunk && frontChunk->Items[x][y][0].IsTranslucent())
							m_DrawList.AddSolid({ x, y, z }, vertices_front_index, Items[x][y][z]);
					}

					if (checkBottom && Items[x][y - 1][z].IsTranslucent())
						m_DrawList.AddSolid({ x, y, z }, vertices_bottom_index, Items[x][y][z]);

					if (checkTop    && Items[x][y + 1][z].IsTranslucent())
						m_DrawList.AddSolid({ x, y, z }, vertices_top_index,    Items[x][y][z]);

					if (checkFront  && Items[x][y][z + 1].IsTranslucent())
						m_DrawList.AddSolid({ x, y, z }, vertices_front_index,  Items[x][y][z]);

					if (checkRight  && Items[x + 1][y][z].IsTranslucent())
						m_DrawList.AddSolid({ x, y, z }, vertices_right_index,  Items[x][y][z]);

					if (checkBehind && Items[x][y][z - 1].IsTranslucent())
						m_DrawList.AddSolid({ x, y, z }, vertices_behind_index, Items[x][y][z]);

					if (checkLeft   && Items[x - 1][y][z].IsTranslucent())
						m_DrawList.AddSolid({ x, y, z }, vertices_left_index,   Items[x][y][z]);
				}
			}
		}			

		m_NeedToRecreate = false;
		m_DrawList.EndRecreating();
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

	glm::vec3 Chunk::CalculatePosition(const glm::vec3& position)
	{
		return { position.x - std::fmod(position.x, chunk_size_XZ), 0.0f, position.z - std::fmod(position.z, chunk_size_XZ) };
	}

	Chunk* Chunk::Create(const glm::vec3& position)
	{
		return new Chunk(CalculatePosition(position));
	}

}