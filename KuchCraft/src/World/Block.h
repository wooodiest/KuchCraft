#pragma once

namespace KuchCraft {

	enum class BlockType
	{
		Air = 0,
		Bedrock, Bricks,
		CoalOre, Cobblestone, CraftingTable,
		DiamondOre, Dioryte, Dirt,
		Furnace,
		Granite, Grass, Gravel,
		IronOre,
		OakLog, OakPlanks,
		Sand, Stone, StoneBrick,

		None // No element and/or last element
	};

	class Block
	{
	public:
		Block() = default;
		Block(const BlockType& type);
		~Block() = default;

		BlockType blockType = BlockType::Air;
		static bool IsTranspaent(const Block& block);

		operator BlockType()                     const { return  blockType;                    }
		bool operator ==(const Block& other)     const { return  blockType == other.blockType; }
		bool operator !=(const Block& other)     const { return !(*this == other);             }
		bool operator ==(const BlockType& other) const { return  blockType == other;           }
		bool operator !=(const BlockType& other) const { return !(*this == other);             }
	};

}