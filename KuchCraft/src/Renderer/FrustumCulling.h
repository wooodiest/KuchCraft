#pragma once

#include "Camera.h"
#include "World/Chunk.h"

namespace KuchCraft {

	class FrustumCulling
	{
	public:
		static void GetChunksToDraw(std::vector<Chunk*>& chunksToDraw, std::vector<Chunk*>& activeChunks, const Camera& camera);

	};

}