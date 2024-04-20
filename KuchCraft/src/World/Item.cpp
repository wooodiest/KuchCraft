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
        {
            constexpr float block_size = 1.0f;
            return AABB(
                  position, 
                { position.x + block_size, position.y + block_size, position.z + block_size }
            );
        }

        if (IsFoliageQuad())
        {
            constexpr float block_size = 1.0f;
            constexpr float quad_size  = 0.25f;
            return AABB(
                { position.x + quad_size,              position.y,                          position.z + quad_size              },
                { position.x + block_size - quad_size, position.y + block_size - quad_size, position.z + block_size - quad_size }
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