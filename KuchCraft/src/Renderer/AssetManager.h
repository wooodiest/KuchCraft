#pragma once

#include "Renderer/AssetManagerData.h"

namespace KuchCraft {

	class AssetManager
	{
	public:
		static Ref<Texture2D> GetBlockTexture(BlockType type);
		static Ref<Texture2D> GetBlockTexture(const Block& block);

	private:
		static void Init();
		static void ShutDown();

		static void PrepareBlocks();

	public:
		friend class Renderer;

	private:
		static AssetManagerBlockData s_BlockData;

	private:
		AssetManager() = default;

	};

}