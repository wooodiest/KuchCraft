#pragma once

#include "World/ItemInfo.h"
#include "Physics/AABB.h"

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

		const std::string& GetName() const { return s_ItemNames[Type]; }

		AABB GetAABB(const glm::ivec3& position) const;

		// x,z - position; y - rotation
		glm::vec3 GetPositionAndRotationOffset(const glm::ivec3& position) const;

	public:
		inline bool IsSolidBlock()       const { return (uint32_t)Type >= item_type_solid_block_begin       && (uint32_t)Type <= item_type_solid_block_end;       }
		inline bool IsTransparentBlock() const { return (uint32_t)Type >= item_type_transparent_block_begin && (uint32_t)Type <= item_type_transparent_block_end; }
		inline bool IsFoliageQuad()      const { return (uint32_t)Type >= item_foliage_quad_begin           && (uint32_t)Type <= item_foliage_quad_end;           }

		inline bool IsSapling()  const { return (uint32_t)Type >= item_foliage_quad_saplings_begin  && (uint32_t)Type <= item_foliage_quad_saplings_end;  }
		inline bool IsCrop()     const { return (uint32_t)Type >= item_foliage_quad_crops_begin     && (uint32_t)Type <= item_foliage_quad_crops_end;     }
		inline bool IsFlower()   const { return (uint32_t)Type >= item_foliage_quad_flowers_begin   && (uint32_t)Type <= item_foliage_quad_flowers_end;   }
		inline bool IsMushroom() const { return (uint32_t)Type >= item_foliage_quad_mushrooms_begin && (uint32_t)Type <= item_foliage_quad_mushrooms_end; }

		inline bool IsTranslucent() const { return Type == ItemType::Air || IsTransparentBlock() || IsFoliageQuad(); }
		inline bool IsPhysical()    const { return IsSolidBlock(); }

	public:
		static std::map<ItemType, std::string> s_ItemNames;

	};

}