#include "kcpch.h"
#include "World/Item.h"

#include "World/ItemData.h"

#include "WorldData.h"

namespace KuchCraft {

    std::map<ItemType, std::string> Item::s_ItemNames = Utils::CreateEnumStringMap<ItemType>();

    Item::Item()
        : Type(ItemType::Air), Rotation(ItemRotation::Deg0)
    {
    }

    Item::Item(ItemType type)
        : Type(type), Rotation(ItemRotation::Deg0)
    {
    }

    Item::Item(ItemType type, ItemRotation rotation)
        : Type(type), Rotation(rotation)
    {
    }

    Item::~Item()
    {
    }

    AABB Item::GetAABB(const glm::ivec3& position) const
    {
        glm::vec3 offsets = GetPositionAndRotationOffset(position);
        glm::vec3 pos = glm::vec3(position) + glm::vec3{ offsets.x, 0.0f, offsets.z };

        if (IsSolidBlock())
            return AABB(
                { pos.x,        pos.y,        pos.z        },
                { pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f }
            );
        
        if (IsFoliageQuad())
        {
            if (IsSapling())
                return AABB(
                    { pos.x + 0.3f, pos.y,         pos.z + 0.3f },
                    { pos.x + 0.7f, pos.y + 0.75f, pos.z + 0.7f }
            );

            if (IsCrop())
                return AABB(
                    { pos.x + 0.1f, pos.y,        pos.z + 0.1f },
                    { pos.x + 0.9f, pos.y + 0.6f, pos.z + 0.9f }
                );
            
            if (IsFlower())
                return AABB(
                    { pos.x + 0.25f, pos.y,         pos.z + 0.25f },
                    { pos.x + 0.75f, pos.y + 0.75f, pos.z + 0.75f }
                );

            if (IsMushroom())
                return AABB(
                    { pos.x + 0.25f, pos.y,        pos.z + 0.25f },
                    { pos.x + 0.75f, pos.y + 0.5f, pos.z + 0.75f }
                );

            return AABB(
                { pos.x + 0.1f, pos.y,        pos.z + 0.1f },
                { pos.x + 0.9f, pos.y + 0.9f, pos.z + 0.9f }
            );
        }

        return AABB();
    }

    glm::vec3 Item::GetPositionAndRotationOffset(const glm::ivec3& position) const
    {
        uint32_t x = static_cast<uint32_t>(std::fmod(position.x, random_offset_size_XZ));
        uint32_t z = static_cast<uint32_t>(std::fmod(position.z, random_offset_size_XZ));

        if (IsFoliageQuad() && !IsCrop())
        {
            glm::vec3 output = random_position_offsets_XZ[x * z];
            output.y = random_rotation_offsets[x * z];
            return output;
        }

        return { 0.0f, 0.0f, 0.0f };
    }
 
}