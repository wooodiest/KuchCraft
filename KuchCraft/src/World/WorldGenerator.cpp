#include "WorldGenerator.h"

#include "Core/Random.h"

namespace KuchCraft {

	void WorldGenerator::Init()
	{

	}
	void WorldGenerator::FillWithRandomBlocks(Chunk* chunk)
	{
		int count = (int)BlockType::None - 1;
		int block = Random::Int(1, count);

		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
				{
					if (y == 0)
						chunk->blocks[x][y][z].blockType = BlockType::Bedrock;
					else
					{
						if (y < 60)
						{
							if (Random::Int(0, 1000) == 0)
								chunk->blocks[x][y][z].blockType = BlockType::Air;
							else
								chunk->blocks[x][y][z].blockType = static_cast<BlockType>(block);
						}
						else
							chunk->blocks[x][y][z].blockType = BlockType::Air;
					}

				}
			}
		}
	}
}