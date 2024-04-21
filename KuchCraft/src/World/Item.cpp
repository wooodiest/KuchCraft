#include "kcpch.h"
#include "World/Item.h"

#include <magic_enum/magic_enum.hpp>

namespace KuchCraft {

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
        if (IsSolidBlock())
            return AABB(
                { position.x,        position.y,        position.z        },
                { position.x + 1.0f, position.y + 1.0f, position.z + 1.0f }
            );
        
        if (IsFoliageQuad())
        {
            if (IsSapling())
                return AABB(
                    { position.x + 0.3f, position.y,          position.z + 0.3f },
                    { position.x + 0.7f, position.y + 0.75f,  position.z + 0.7f }
            );

            if (IsCrop())
                return AABB(
                    { position.x + 0.1f, position.y,         position.z + 0.1f },
                    { position.x + 0.9f, position.y + 0.6f,  position.z + 0.9f }
                );
            
            if (IsFlower())
                return AABB(
                    { position.x + 0.25f, position.y,         position.z + 0.25f },
                    { position.x + 0.75f, position.y + 0.75f, position.z + 0.75f }
                );

            if (IsMushroom())
                return AABB(
                    { position.x + 0.25f, position.y,        position.z + 0.25f },
                    { position.x + 0.75f, position.y + 0.5f, position.z + 0.75f }
                );

            return AABB(
                { position.x + 0.1f, position.y,         position.z + 0.1f },
                { position.x + 0.9f, position.y + 0.9f,  position.z + 0.9f }
            );
        }

        return AABB();
    }

    // Set up item names
    // ItemType::GoldBlock -> gold block
	std::map<ItemType, std::string> Item::s_ItemNames = []() {
        std::map<ItemType, std::string> map;
        for (auto value : magic_enum::enum_values<ItemType>())
        {
            const std::string input = std::string(magic_enum::enum_name(value));
            std::string output;
            for (size_t i = 0; i < input.length(); ++i)
            {
                char currentChar = input[i];
                if (std::isupper(currentChar))
                {
                    if (i > 0 && !std::isspace(input[i - 1]) && !std::isspace(input[i]))
                        output += ' ';
                    
                    output += std::tolower(currentChar);
                }
                else
                    output += currentChar;
            }

            map[value] = output;
        }
        return map;
    }();
}