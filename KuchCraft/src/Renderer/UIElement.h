#pragma once

#include <magic_enum/magic_enum.hpp>

namespace KuchCraft {

	enum class UIElement
	{
		Hotbar,
		HotbarSelected,

		Crosshair,
		Cursor,

		HeartEmpty,
		HeartFull,
		FoodEmpty,
		FoodFull,
		ArmorEmpty,
		ArmorFull,

		Inventory,
		Slot,

		None
	};

}

template <>
struct magic_enum::customize::enum_range<KuchCraft::UIElement> {
	static constexpr int min = 0;
	static constexpr int max = (int)KuchCraft::UIElement::None;
};