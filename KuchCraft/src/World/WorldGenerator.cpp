#include "WorldGenerator.h"

#include "Core/Random.h"
#include <iostream>

namespace KuchCraft {

	siv::PerlinNoise::seed_type WorldGenerator::s_Seed;
	siv::PerlinNoise WorldGenerator::s_Noise;
	static uint32_t s_SeeLevel = 60;

	void WorldGenerator::Init(uint64_t seed)
	{
		s_Seed = seed;
		s_Noise = siv::PerlinNoise{ s_Seed };
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
		auto& position = chunk->GetPosition();

		double A = 0.005;
		double B = 0.02;
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int z = 0; z < chunk_size_XZ; z++)
			{
				double octaves = 1 + 5 * s_Noise.noise2D_01((position.x + x) * 0.01, (position.z + z) * 0.01);
				double H = 50 * s_Noise.octave2D((position.x + x) * B, (position.z + z) * B, octaves);

				int height = 5 + s_SeeLevel + H * s_Noise.octave2D((position.x + x) * A, (position.z + z) * A, 4);
				for (int y = 0; y < chunk_size_Y; y++)
				{
					if (y < height)
						chunk->Blocks[x][y][z] = Block(BlockType::Grass);
				}
			}
		}
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y - 1; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
				{
					if (y == 0)
						chunk->Blocks[x][y][z] = Block(BlockType::Bedrock);
					else if (y < s_SeeLevel - 1)
					{
						if (chunk->Blocks[x][y + 1][z].blockType == BlockType::Air)
						{
							chunk->Blocks[x][y + 1][z] = Block(BlockType::Water);
						}
					}
				}
			}
			
		}
#if 1
		for (int x = 2; x < chunk_size_XZ - 2; x++)
		{
			for (int y = 1; y < chunk_size_Y - 8; y++)
			{
				for (int z = 2; z < chunk_size_XZ - 2; z++)
				{
					if (y < 70)
					{
						double H = s_Noise.octave2D((position.x + x), (position.z + z), 5);
						if (
							chunk->Blocks[x][y - 1][z].blockType == BlockType::Grass
							&& chunk->Blocks[x][y + 1][z].blockType == BlockType::Air
							&& chunk->Blocks[x][y][z].blockType != BlockType::Water
							&& H > 0.65)
						{
							chunk->Blocks[x][y][z] = Block(BlockType::OakLog);
							chunk->Blocks[x][y + 1][z] = Block(BlockType::OakLog);
							chunk->Blocks[x][y + 2][z] = Block(BlockType::OakLog);
							chunk->Blocks[x][y + 3][z] = Block(BlockType::OakLog);

							chunk->Blocks[x][y + 4][z] = Block(BlockType::Leaves);
							chunk->Blocks[x - 1][y + 4][z] = Block(BlockType::Leaves);
							chunk->Blocks[x + 1][y + 4][z] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 4][z - 1] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 4][z + 1] = Block(BlockType::Leaves);

							chunk->Blocks[x][y + 5][z] = Block(BlockType::Leaves);
							chunk->Blocks[x - 1][y + 5][z] = Block(BlockType::Leaves);
							chunk->Blocks[x + 1][y + 5][z] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 5][z - 1] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 5][z + 1] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 5][z] = Block(BlockType::Leaves);
							chunk->Blocks[x - 2][y + 5][z] = Block(BlockType::Leaves);
							chunk->Blocks[x + 2][y + 5][z] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 5][z - 2] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 5][z + 2] = Block(BlockType::Leaves);
							chunk->Blocks[x - 1][y + 5][z - 1] = Block(BlockType::Leaves);
							chunk->Blocks[x + 1][y + 5][z + 1] = Block(BlockType::Leaves);
							chunk->Blocks[x - 1][y + 5][z + 1] = Block(BlockType::Leaves);
							chunk->Blocks[x + 1][y + 5][z - 1] = Block(BlockType::Leaves);

							chunk->Blocks[x][y + 6][z] = Block(BlockType::Leaves);
							chunk->Blocks[x - 1][y + 6][z] = Block(BlockType::Leaves);
							chunk->Blocks[x + 1][y + 6][z] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 6][z - 1] = Block(BlockType::Leaves);
							chunk->Blocks[x][y + 6][z + 1] = Block(BlockType::Leaves);

							chunk->Blocks[x][y + 7][z] = Block(BlockType::Leaves);

						}
					}
						
				}
			}			
		}
#endif

	}

}