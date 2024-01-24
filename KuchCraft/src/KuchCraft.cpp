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
		auto ch = m_World.GetChunk({ 0.0f, 0.0f,0.0f });
		ch->m_Blocks[5][5][5].m_BlockType = BlockType::Air;
		ch->m_Blocks[5][5][6].m_BlockType = BlockType::Air;
		ch->m_Blocks[5][5][7].m_BlockType = BlockType::Air;

		ch->m_Blocks[0][0][0].m_BlockType = BlockType::Air;
		ch->m_Blocks[1][0][0].m_BlockType = BlockType::Air;
		ch->m_Blocks[0][0][1].m_BlockType = BlockType::Air;
		ch->m_Blocks[1][1][1].m_BlockType = BlockType::Air;

		ch->m_Blocks[5][3][0].m_BlockType = BlockType::Air;
		ch->m_Blocks[6][3][0].m_BlockType = BlockType::Air;
		ch->m_Blocks[7][3][0].m_BlockType = BlockType::Air;
		ch->m_Blocks[5][4][0].m_BlockType = BlockType::Air;
		ch->m_Blocks[6][4][0].m_BlockType = BlockType::Air;
		ch->m_Blocks[7][4][0].m_BlockType = BlockType::Air;
#if 1
		for (int x = 0; x < 16; x++)
		{
			for (int y = 0; y < 128; y++)
			{
				for (int z = 0; z < 16; z++)
				{
					if (y > 45)
					{
						ch->m_Blocks[x][y][z].m_BlockType = BlockType::Stone;
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