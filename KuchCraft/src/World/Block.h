#pragma once

namespace KuchCraft {

	enum class BlockType : uint8_t
	{
		Air = 0, Water,
		Bedrock, Bricks,
		CoalOre, Cobblestone, CraftingTable,
		DiamondOre, Dioryte, Dirt,
		Furnace,
		Granite, Grass, Gravel,
		IronOre,
		OakLog, OakPlanks,
		Sand, Stone, StoneBrick,
		Leaves,

		None // No element and/or last element
	};
	constexpr uint32_t first_index_of_block_types     = (uint32_t)BlockType::Water;
	constexpr uint32_t total_number_of_block_types    = (uint32_t)BlockType::None - 1;
	constexpr uint32_t absolute_number_of_block_types = (uint32_t)BlockType::None;

	struct Block
	{
		BlockType blockType = BlockType::Air;

		Block() = default;
		Block(BlockType type)
			: blockType(type) {}

		operator BlockType()                     const { return  blockType;                    }
		bool operator ==(const Block& other)     const { return  blockType == other.blockType; }
		bool operator !=(const Block& other)     const { return !(*this == other);             }
		bool operator ==(const BlockType& other) const { return  blockType == other;           }
		bool operator !=(const BlockType& other) const { return !(*this == other);             }

		bool IsTransparent() const { return IsTransparent(blockType); }
		bool IsSolid()       const { return IsSolid(blockType);       }

		const std::string& GetName() const { return GetName(*this); };

		static bool IsTransparent(const Block& block);
		static bool IsSolid(const Block& block);
		static const std::string& GetName(const Block& block);

	private:
		static std::unordered_map<BlockType, std::string> s_BlockNames;
	};

}