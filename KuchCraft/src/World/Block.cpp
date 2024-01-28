#include "Block.h"

namespace KuchCraft {

	Block::Block(const BlockType& type)
		: blockType(type)
	{
	}

	bool Block::IsTranspaent(const Block& block)
	{
		switch (block.blockType)
		{
			case BlockType::Air:  return true;
			case BlockType::None: return true;
		}
		return false;
	}
}