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

					bool isWater = Items[x][y][z].Type == ItemType::Water;
					if (Items[x][y][z].IsTransparentBlock() && !isWater)
					{
						m_DrawList.AddTransparent({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, Items[x][y][z]);
						continue;
					}

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
						if (leftChunk)
						{
							Item leftItem      = leftChunk->Items[chunk_size_XZ - 1][y][z];
							bool isTranslucent = leftItem.IsTranslucent();
							if (isTranslucent)
							{
								if (isWater)
								{
									if (leftItem.Type != ItemType::Water)
										m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_left_index);
								}
								else
									m_DrawList.AddSolid({ x, y, z }, vertices_left_index, Items[x][y][z]);
							}
						}
					}
					else if (x == chunk_size_XZ - 1)
					{
						checkRight = false;
						if (rightChunk)
						{
							Item rightItem     = rightChunk->Items[0][y][z];
							bool isTranslucent = rightItem.IsTranslucent();
							if (isTranslucent)
							{
								if (isWater)
								{
									if (rightItem.Type != ItemType::Water)
										m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_right_index);
								}
								else
									m_DrawList.AddSolid({ x, y, z }, vertices_right_index, Items[x][y][z]);
							}
						}
					}

					if (z == 0)
					{
						checkBehind = false;
						if (behindChunk)
						{
							Item behindItem    = behindChunk->Items[x][y][chunk_size_XZ - 1];
							bool isTranslucent = behindItem.IsTranslucent();
							if (isTranslucent)
							{
								if (isWater)
								{
									if (behindItem.Type != ItemType::Water)
										m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_behind_index);
								}
								else
									m_DrawList.AddSolid({ x, y, z }, vertices_behind_index, Items[x][y][z]);
							}
						}
					}
					else if (z == chunk_size_XZ - 1)
					{
						checkFront = false;
						if (frontChunk)
						{
							Item frontItem     = frontChunk->Items[x][y][0];
							bool isTranslucent = frontItem.IsTranslucent();
							if (isTranslucent)
							{
								if (isWater)
								{
									if (frontItem.Type != ItemType::Water)
										m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_front_index);
								}
								else
									m_DrawList.AddSolid({ x, y, z }, vertices_front_index, Items[x][y][z]);
							}
						}
					}

					if (checkBottom)
					{
						Item bottomItem    = Items[x][y - 1][z];
						bool isTranslucent = bottomItem.IsTranslucent();
						if (isTranslucent)
						{
							if (isWater)
							{
								if (bottomItem.Type != ItemType::Water)
									m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_bottom_index);
							}
							else
								m_DrawList.AddSolid({ x, y, z }, vertices_bottom_index, Items[x][y][z]);
						}
					}
					
					if (checkTop)
					{
						Item topItem       = Items[x][y + 1][z];
						bool isTranslucent = topItem.IsTranslucent();
						if (isTranslucent)
						{
							if (isWater)
							{
								if (topItem.Type != ItemType::Water)
									m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_top_index);
							}
							else
								m_DrawList.AddSolid({ x, y, z }, vertices_top_index, Items[x][y][z]);
						}
					}
				
					if (checkBehind)
					{
						Item behindItem    = Items[x][y][z - 1];
						bool isTranslucent = behindItem.IsTranslucent();
						if (isTranslucent)
						{
							if (isWater)
							{
								if (behindItem.Type != ItemType::Water)
									m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_behind_index);
							}
							else
								m_DrawList.AddSolid({ x, y, z }, vertices_behind_index, Items[x][y][z]);
						}
					}

					if (checkFront)
					{
						Item frontItem     = Items[x][y][z + 1];
						bool isTranslucent = frontItem.IsTranslucent();
						if (isTranslucent)
						{
							if (isWater)
							{
								if (frontItem.Type != ItemType::Water)
									m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_front_index);
							}
							else
								m_DrawList.AddSolid({ x, y, z }, vertices_front_index, Items[x][y][z]);
						}
					}

					if (checkLeft)
					{
						Item leftItem      = Items[x - 1][y][z];
						bool isTranslucent = leftItem.IsTranslucent();
						if (isTranslucent)
						{
							if (isWater)
							{
								if (leftItem.Type != ItemType::Water)
									m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_left_index);
							}
							else
								m_DrawList.AddSolid({ x, y, z }, vertices_left_index, Items[x][y][z]);
						}
					}

					if (checkRight)
					{
						Item rightItem     = Items[x + 1][y][z];
						bool isTranslucent = rightItem.IsTranslucent();
						if (isTranslucent)
						{
							if (isWater)
							{
								if (rightItem.Type != ItemType::Water)
									m_DrawList.AddWater({ m_Position.x + x, m_Position.y + y, m_Position.z + z }, vertices_right_index);
							}
							else
								m_DrawList.AddSolid({ x, y, z }, vertices_right_index, Items[x][y][z]);
						}
					}			
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