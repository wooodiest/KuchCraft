#pragma once

#include "Renderer/AssetManagerData.h"

namespace KuchCraft {

	class AssetManager
	{
	public:
		static Texture2D& GetBlockTexture(BlockType type) { return s_BlockData.Textures[type]; }

		static Texture2D& GetHotbarTexture()          { return s_UIData.HotbarTexture;          }
		static Texture2D& GetHotbarSelectionTexture() { return s_UIData.HotbarSelectionTexture; }
		static Texture2D& GetCrosshairTexture()       { return s_UIData.CrosshairTexture;       }

		static Texture2D& GetHeartEmptyTexture() { return s_UIData.HeartEmptyTexture; }
		static Texture2D& GetHeartFullTexture()  { return s_UIData.HeartFullTexture;  }

		static Texture2D& GetFoodEmptyTexture() { return s_UIData.FoodEmptyTexture; }
		static Texture2D& GetFoodFullTexture()  { return s_UIData.FoodFullTexture;  }

		static Texture2D& GetArmorEmptyTexture() { return s_UIData.ArmorEmptyTexture; }
		static Texture2D& GetArmorFullTexture()  { return s_UIData.ArmorFullTexture; }

	private:
		static void Init();
		static void ShutDown();

		static void PrepareBlocks();
		static void PrepareUI();

	public:
		friend class Renderer;

	private:
		static AssetManagerBlockData s_BlockData;
		static AssetManagerUIData    s_UIData;

	private:
		AssetManager() = default;

	};

}