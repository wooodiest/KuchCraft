#pragma once

#include "World/ItemInfo.h"

namespace KuchCraft {

	class Item
	{
	public:
		ItemType     Type;
		ItemRotation Rotation;

	public:
		Item();
		Item(ItemType type);
		Item(ItemType type, ItemRotation rotation);
		~Item();

	public:
		inline bool IsSolidBlock()       const { return (uint32_t)Type >= item_type_solid_block_begin       && (uint32_t)Type <= item_type_solid_block_end;       }
		inline bool IsTransparentBlock() const { return (uint32_t)Type >= item_type_transparent_block_begin && (uint32_t)Type <= item_type_transparent_block_end; }
		inline bool IsFoliageQuad()      const { return (uint32_t)Type >= item_foliage_quad_begin           && (uint32_t)Type <= item_foliage_quad_end;           }

	public:
		static std::map<ItemType, std::string> s_ItemNames;

	};

}