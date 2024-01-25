#include "KuchCraft.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include <iostream>
#include "Core/Random.h"

namespace KuchCraft {

	KuchCraft::KuchCraft()
	{
#if 0 // Show all blocks
		auto ch = m_World.GetChunk({ 0.0f, 0.0f,0.0f });
		int count = (int)BlockType::LastElement - 1;
		for (int x = 0; x < chunk_size_X; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_Z; z++)
				{
					if (x % 2 == 0 && y % 2 == 0 && z % 2 == 0)
					{
						ch->m_Blocks[x][y][z].m_BlockType = static_cast<BlockType>(count);
						count--;
						if (count == 0)
							return;

					}
				}
			}
		}
#endif

#if 1 // Chunk of random blocks
		auto ch = m_World.GetChunk({ 0.0f, 0.0f,0.0f });
		int count = (int)BlockType::LastElement - 1;
		for (int x = 0; x < chunk_size_X; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_Z; z++)
				{	
					ch->m_Blocks[x][y][z].m_BlockType = static_cast<BlockType>(Random::Int(0, count));		
				}
			}
		}

#endif

	}

	KuchCraft::~KuchCraft()
	{

	}

	void KuchCraft::Init()
	{

	}

	void KuchCraft::OnUpdate(float dt)
	{
		m_Camera.OnUpdate(dt);

		m_Renderer.BeginScene(m_Camera);

		m_World.GetChunk({ 0.0f, 0.0f,0.0f })->Render();

		m_Renderer.EndScene();
	}

	void KuchCraft::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		Renderer::Get().OnViewportSizeChanged(width, height);
	}

}