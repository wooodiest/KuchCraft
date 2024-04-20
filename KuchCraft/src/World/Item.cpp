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