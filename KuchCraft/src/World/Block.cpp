#include "Block.h"

namespace KuchCraft {

	bool Block::IsTranspaent(const Block& block)
	{
		switch (block.blockType)
		{
			case BlockType::Air:    return true;
			case BlockType::Water:  return true;
			case BlockType::None:   return true;
		}
		return false;
	}
}