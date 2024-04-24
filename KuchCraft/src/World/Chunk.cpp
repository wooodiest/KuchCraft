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
					bool checkBottom = (y > 0);
					bool checkTop    = (y < chunk_size_Y - 1);
					bool checkFront  = (z < chunk_size_XZ - 1);
					bool checkBehind = (z > 0);
					bool checkRight  = (x < chunk_size_XZ - 1);
					bool checkLeft   = (x > 0);

					if (!checkLeft  && leftChunk)
						AddToDrawList({ x, y, z }, leftChunk->Items[chunk_size_XZ - 1][y][z],   vertices_left_index,   isWater);
					if (!checkRight && rightChunk)											    					   
						AddToDrawList({ x, y, z }, rightChunk->Items[0][y][z],                  vertices_right_index,  isWater);

					if (!checkBehind && behindChunk)
						AddToDrawList({ x, y, z }, behindChunk->Items[x][y][chunk_size_XZ - 1], vertices_behind_index, isWater);
					if (!checkFront  && frontChunk)
						AddToDrawList({ x, y, z }, frontChunk->Items[x][y][0],                  vertices_front_index,  isWater);

					if (!checkTop)
						AddToDrawList({ x, y, z }, Item(ItemType::Air), vertices_top_index, isWater);

					if (checkBottom)
						AddToDrawList({ x, y, z }, Items[x][y - 1][z], vertices_bottom_index, isWater);				
					if (checkTop)
						AddToDrawList({ x, y, z }, Items[x][y + 1][z], vertices_top_index,    isWater);
				
					if (checkBehind)
						AddToDrawList({ x, y, z }, Items[x][y][z - 1], vertices_behind_index, isWater);			
					if (checkFront)
						AddToDrawList({ x, y, z }, Items[x][y][z + 1], vertices_front_index,  isWater);

					if (checkLeft)
						AddToDrawList({ x, y, z }, Items[x - 1][y][z], vertices_left_index,   isWater);
					if (checkRight)
						AddToDrawList({ x, y, z }, Items[x + 1][y][z], vertices_right_index,  isWater);			
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

	void Chunk::AddToDrawList(const glm::ivec3& position, Item neighbourItem, uint32_t verticesIndex, bool isWater)
	{
		if (neighbourItem.IsTranslucent())
		{
			if (isWater)
			{
				if (neighbourItem.Type != ItemType::Water)
					m_DrawList.AddWater(m_Position + glm::vec3{ position }, verticesIndex);
			}
			else
				m_DrawList.AddSolid(position, verticesIndex, Items[position.x][position.y][position.z]);
		}
	}

}