#include "KuchCraft.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include <iostream>

namespace KuchCraft {

	KuchCraft::KuchCraft()
	{
#if 1 // Show all blocks
		auto ch = m_World.GetChunk({ 0.0f, 0.0f,0.0f });
		int count = (int)BlockType::LastElement - 1;
		for (int x = 0; x < 16; x++)
		{
			for (int y = 0; y < 128; y++)
			{
				for (int z = 0; z < 16; z++)
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