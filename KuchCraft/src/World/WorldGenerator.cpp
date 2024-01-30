#include "WorldGenerator.h"

#include "Core/Random.h"

namespace KuchCraft {

	uint64_t WorldGenerator::s_Seed = 0;

	void WorldGenerator::Init(uint64_t seed)
	{
		s_Seed = seed;
	}

	void WorldGenerator::ShutDown()
	{
		// Clear 
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
						chunk->Blocks[x][y][z].blockType = BlockType::Bedrock;
					else
					{
						if (y < 60)
						{
							if (Random::Int(0, 1000) == 0)
								chunk->Blocks[x][y][z].blockType = BlockType::Air;
							else
								chunk->Blocks[x][y][z].blockType = static_cast<BlockType>(block);
						}
						else
							chunk->Blocks[x][y][z].blockType = BlockType::Air;
					}

				}
			}
		}
	}

	void WorldGenerator::Generate(Chunk* chunk)
	{
		const int terainLevel = 60;

		// Fill bottom with bedrock
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int z = 0; z < chunk_size_XZ; z++)
			{
				chunk->Blocks[x][0][z] = Block(BlockType::Bedrock);
			}
		}

		auto& position = chunk->GetPosition();

		float frequency1 = 0.1f;
		float frequency2 = 0.15f;
		float frequency3 = 0.069f;
		float amplitude1 = 5.0f;
		float amplitude2 = 5.0f;
		float amplitude3 = 3.0f;
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int z = 0; z < chunk_size_XZ; z++)
			{
				float xOffset  = glm::sin((position.x + x) * frequency1) * amplitude1 + glm::sin((position.x + x) * frequency2) + (glm::cos(position.x + x) * frequency3 * amplitude3) * (glm::cos(position.x + x) * frequency3 * amplitude3);
				float zOffset  = glm::sin((position.z + z) * frequency1) * amplitude1 + glm::sin((position.z + z) * frequency2) + (glm::cos(position.z + z) * frequency3 * amplitude3) * (glm::cos(position.z + z) * frequency3 * amplitude3);
				float surfaceY = terainLevel + xOffset + zOffset;
				for (int y = 1; y < chunk_size_Y; y++)
				{
					if (y <= surfaceY)
					{
						chunk->Blocks[x][y][z] = Block(BlockType::Grass);
					}

				}
			}	
		}

	}



}
