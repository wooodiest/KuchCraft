#include "kcpch.h"
#include "Renderer/Renderer3D/Renderer3D.h"

#include "Core/Application.h"
#include "Core/Utils.h"

#include "Renderer/Renderer.h"
#include "Renderer/AssetManager.h"
#include "Renderer/RendererCommand.h"

#include "Renderer/Utils/FrustumCulling.h"

namespace KuchCraft {

	Renderer3DData              Renderer3D::s_Data;
	Renderer3DInfo              Renderer3D::s_Info;
	Renderer3DChunkData         Renderer3D::s_ChunkData;
	Renderer3DSkyboxData        Renderer3D::s_SkyboxData;
	Renderer3DWaterData         Renderer3D::s_WaterData;
	Renderer3DOutlinedBlockData Renderer3D::s_OutlinedBlockData;
	Renderer3DTintedData        Renderer3D::s_TintedData;
	Renderer3DQuadData          Renderer3D::s_QuadData;
	Renderer3DCubeData          Renderer3D::s_CubeData;
	Renderer3DTextInfo          Renderer3D::s_TextInfo;
	Renderer3DTextData          Renderer3D::s_TextData;

	void Renderer3D::Init()
	{
		LoadRenderer3DInfo();

		PrepareChunks();
		PrepareSkybox();
		PrepareWater();
		PrepareOutlinedBlock();
		PrepareTinted();
		PrepareTextRendering();
		PrepareQuads();
		PrepareCubes();

		// QuadIndexBuffer
		uint32_t  indexBufferElementCount = s_Info.MaxIndices > max_indices_in_chunk ? s_Info.MaxIndices : max_indices_in_chunk;
		uint32_t* indices = new uint32_t[indexBufferElementCount];
		uint32_t  offset = 0;
		for (uint32_t i = 0; i < indexBufferElementCount; i += quad_index_count)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}
		s_Data.QuadIndexBuffer.Create(indices, indexBufferElementCount);
		delete[] indices;

		{
			FrameBufferSpecification frameBufferSpecification;
			frameBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::DEPTH24STENCIL8 };
			frameBufferSpecification.Width = Application::Get().GetWindow().GetWidth();
			frameBufferSpecification.Height = Application::Get().GetWindow().GetHeight();
			s_Data.RenderFrameBuffer.Create(frameBufferSpecification);
		}

		{
			FrameBufferSpecification frameBufferSpecification;
			frameBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8 };
			frameBufferSpecification.Width  = Application::Get().GetWindow().GetWidth();
			frameBufferSpecification.Height = Application::Get().GetWindow().GetHeight();
			s_TintedData.FrameBuffer.Create(frameBufferSpecification);
		}
	}

	void Renderer3D::ShutDown()
	{
		delete s_QuadData.TextureSlots;
		delete s_CubeData.TextureSlots;
	}

	void Renderer3D::LoadRenderer3DInfo()
	{
		s_Info     = Renderer3DInfo();
		s_TextInfo = Renderer3DTextInfo();
	}

	void Renderer3D::Render()
	{
		Renderer::s_Stats.Renderer3DTimer.Start();

		s_Data.RenderFrameBuffer.BindAndClear();

		FrustumCulling::Chunks(s_ChunkData.Chunks, *Renderer::s_Data.CurrentCamera, s_ChunkData.ChunksToRender);
		Renderer::s_Stats.ChunksToRender = s_ChunkData.ChunksToRender.size();

		RenderChunks();
		RenderSkybox();

		if (s_OutlinedBlockData.Status)
			RenderOutlinedBlock();

		RenderCubes();
		RenderQuads();

		RenderWater();

		RenderText(); // TODO: think of place to put this and fix depth testing issues

		s_TintedData.FrameBuffer.BindAndClear();
		RenderTinted();

		s_TintedData.FrameBuffer.Unbind();

		Renderer::s_Stats.Renderer3DTimer.Finish();
	}

	void Renderer3D::Clear()
	{
		s_ChunkData.Chunks.        clear();
		s_ChunkData.ChunksToRender.clear();

		s_OutlinedBlockData.Status = false;
		s_TintedData.Tinted        = false;

		s_QuadData.Vertices.clear();
		s_CubeData.Vertices.clear();

		s_TextData.Data.clear();
	}

	void Renderer3D::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		s_Data.RenderFrameBuffer.Resize(width, height);
		s_TintedData.FrameBuffer.Resize(width, height);
	}

	void Renderer3D::PrepareChunks()
	{
		s_ChunkData.VertexArray.Create();
		s_ChunkData.VertexArray.Bind();

		s_ChunkData.VertexBuffer.Create(max_vertices_in_chunk * sizeof(uint32_t));
		s_ChunkData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Uint, "a_PackedData" }
		});

		s_ChunkData.VertexArray.SetVertexBuffer(s_ChunkData.VertexBuffer);

		std::unordered_map<std::string, std::string> chunkShaderData;
		uint32_t maxTextureSlots = Renderer::GetInfo().MaxTextureSlots;
		chunkShaderData["#max_texture_slots"] = std::to_string(maxTextureSlots);

		s_ChunkData.Shader.Create("assets/shaders/chunk.vert.glsl", "assets/shaders/chunk.frag.glsl", chunkShaderData);
		s_ChunkData.Shader.Bind();

		int* samplers = new int[maxTextureSlots];
		for (int i = 0; i < maxTextureSlots; i++)
			samplers[i] = i;
		s_ChunkData.Shader.SetIntArray("u_Textures", samplers, maxTextureSlots);
		delete[] samplers;

		s_ChunkData.VertexArray .Unbind();
		s_ChunkData.VertexBuffer.Unbind();
		s_ChunkData.Shader      .Unbind();
	}

	void Renderer3D::PrepareSkybox()
	{
		s_SkyboxData.VertexArray.Create();
		s_SkyboxData.VertexArray.Bind();

		constexpr float vertices[] = {
			 1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
		};

		s_SkyboxData.VertexBuffer.Create(sizeof(vertices), vertices, true);
		s_SkyboxData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position"}
		});

		s_SkyboxData.VertexArray.SetVertexBuffer(s_SkyboxData.VertexBuffer);

		s_SkyboxData.Shader.Create("assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
		s_SkyboxData.Shader.Bind();
		s_SkyboxData.Shader.SetInt("u_CubemapTexture", 0);

		s_SkyboxData.Texture.Create(CubeMapFacesInfo{
			"assets/skybox/xpos.png",
			"assets/skybox/xneg.png",
			"assets/skybox/ypos.png",
			"assets/skybox/yneg.png",
			"assets/skybox/zpos.png",
			"assets/skybox/zneg.png"
		});

		s_SkyboxData.VertexArray .Unbind();
		s_SkyboxData.VertexBuffer.Unbind();
		s_SkyboxData.Shader      .Unbind();
	}

	void Renderer3D::PrepareWater()
	{
		s_WaterData.VertexArray.Create();
		s_WaterData.VertexArray.Bind();

		s_WaterData.VertexBuffer.Create(max_vertices_in_chunk * sizeof(Vertex_P3C2));
		s_WaterData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_WaterData.VertexArray.SetVertexBuffer(s_WaterData.VertexBuffer);

		s_WaterData.Shader.Create("assets/shaders/water.vert.glsl", "assets/shaders/water.frag.glsl");
		s_WaterData.Shader.Bind();
		s_WaterData.Shader.SetInt("u_Texture", default_texture_slot);

		s_WaterData.VertexArray .Unbind();
		s_WaterData.VertexBuffer.Unbind();
		s_WaterData.Shader      .Unbind();
	}

	void Renderer3D::PrepareOutlinedBlock()
	{
		s_OutlinedBlockData.VertexArray.Create();
		s_OutlinedBlockData.VertexArray.Bind();

		// Float3::pos, Float2::texCoord
		constexpr float vertices[] = {
			// bottom
			1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			// top
			0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			// front
			0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			// right
			1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			// behind
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			// left
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f, 1.0f
		};

		s_OutlinedBlockData.VertexBuffer.Create(sizeof(vertices), vertices, true);
		s_OutlinedBlockData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_OutlinedBlockData.VertexArray.SetVertexBuffer(s_OutlinedBlockData.VertexBuffer);

		std::unordered_map<std::string, std::string> outlinedBlockShaderData;
		outlinedBlockShaderData["#outlined_block_border_radius"] = std::to_string(s_OutlinedBlockData.BorderRadius);
		outlinedBlockShaderData["#outlined_block_border_color"]  = VecToString(s_OutlinedBlockData.Color);

		s_OutlinedBlockData.Shader.Create("assets/shaders/outlined_block.vert.glsl", "assets/shaders/outlined_block.frag.glsl", outlinedBlockShaderData);
		s_OutlinedBlockData.Shader.Bind();

		s_OutlinedBlockData.VertexArray .Unbind();
		s_OutlinedBlockData.VertexBuffer.Unbind();
		s_OutlinedBlockData.Shader      .Unbind();
	}

	void Renderer3D::PrepareTinted()
	{
		s_TintedData.VertexArray.Create();
		s_TintedData.VertexArray.Bind();

		float vertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,

			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f
		};

		s_TintedData.VertexBuffer.Create(sizeof(vertices), vertices, true);
		s_TintedData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_TintedData.VertexArray.SetVertexBuffer(s_TintedData.VertexBuffer);

		s_TintedData.Shader.Create("assets/shaders/tinted.vert.glsl", "assets/shaders/tinted.frag.glsl");
		s_TintedData.Shader.Bind();
		s_TintedData.Shader.SetInt("u_Texture", default_texture_slot);

		s_TintedData.VertexArray .Unbind();
		s_TintedData.VertexBuffer.Unbind();
		s_TintedData.Shader.      Unbind();
	}

	void Renderer3D::PrepareTextRendering()
	{
		s_TextData.VertexArray.Create();
		s_TextData.VertexArray.Bind();

		float vertices[] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
		};

		s_TextData.VertexBuffer.Create(sizeof(vertices), vertices, true);
		s_TextData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" }
		});

		s_TextData.VertexArray.SetVertexBuffer(s_TextData.VertexBuffer);

		std::unordered_map<std::string, std::string> textShaderData;
		textShaderData["#max_text_uniform_array_limit"] = std::to_string(s_TextInfo.MaxCharacterUniformArrayLimit);

		s_TextData.Texture.Create(s_TextInfo.FontPath, s_TextInfo.FontTextureSize, s_TextInfo.FontCharactersCount);

		s_TextData.Shader.Create("assets/shaders/text3D.vert.glsl", "assets/shaders/text3D.frag.glsl", textShaderData);
		s_TextData.Shader.Bind();

		s_TextData.UniformBuffer.Create(s_TextInfo.MaxCharacterUniformArrayLimit * sizeof(Renderer2DUniformText), 2);

		s_TextData.VertexArray.Unbind();
		s_TextData.VertexBuffer.Unbind();
		s_TextData.Shader.Unbind();
	}

	void Renderer3D::RenderChunks()
	{
		Renderer::s_Stats.ChunkTimer.Start();

		RendererCommand::DisableBlending();
		RendererCommand::EnableFaceCulling();
		RendererCommand::EnableDepthTesting();

		s_ChunkData.Shader.     Bind();
		s_ChunkData.VertexArray.Bind();
		s_Data.QuadIndexBuffer. Bind();

		for (const auto& chunk : s_ChunkData.ChunksToRender)
		{
			uint32_t vertexOffset = 0;
			const auto& drawList = chunk->GetDrawList();
			s_ChunkData.Shader.SetFloat3("u_ChunkPosition", chunk->GetPosition());

			for (uint32_t drawCallIndex = 0; drawCallIndex < drawList.GetDrawCallCount(); drawCallIndex++)
			{
				uint32_t indexCount = drawList.GetIndexCount(drawCallIndex);
				if (indexCount != 0)
				{
					uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;
					s_ChunkData.VertexBuffer.SetData(drawList.GetVerticesPtr(vertexOffset), vertexCount * sizeof(uint32_t));

					vertexOffset += vertexCount;

					for (uint32_t textureIndex = 0; textureIndex < drawList.GetTextureCount(drawCallIndex); textureIndex++)
						Texture2D::Bind(drawList.GetTexture(drawCallIndex, textureIndex), textureIndex);

					RendererCommand::DrawElements(indexCount);

					Renderer::s_Stats.DrawCalls++;
					Renderer::s_Stats.Quads += vertexCount / quad_vertex_count;
				}
			}
		}

		Renderer::s_Stats.ChunkTimer.Finish();
	}

	void Renderer3D::RenderSkybox()
	{
		Renderer::s_Stats.SkyboxTimer.Start();

		RendererCommand::DisableBlending();
		RendererCommand::EnableFrontFaceCulling();
		RendererCommand::EnableLessEqualDepthTesting();

		s_SkyboxData.Shader      .Bind();
		s_SkyboxData.VertexArray .Bind();
		s_SkyboxData.VertexBuffer.Bind();
		s_Data.QuadIndexBuffer   .Bind();

		s_SkyboxData.Texture.Bind();
		RendererCommand::DrawElements(cube_face_cout * quad_index_count);

		Renderer::s_Stats.DrawCalls++;
		Renderer::s_Stats.Quads += cube_face_cout;

		Renderer::s_Stats.SkyboxTimer.Finish();
	}

	void Renderer3D::RenderWater()
	{
		Renderer::s_Stats.WaterTimer.Start();

		RendererCommand::EnableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::EnableDepthTesting();

		s_WaterData.Shader     .Bind();
		s_WaterData.VertexArray.Bind();
		s_Data.QuadIndexBuffer .Bind();

		AssetManager::GetItemTexture(ItemType::Water).Bind(default_texture_slot);

		for (const auto& chunk : s_ChunkData.ChunksToRender)
		{
			const auto& drawList = chunk->GetDrawList();
			uint32_t indexCount  = drawList.GetWaterVertices().size() / quad_vertex_count * quad_index_count;

			if (indexCount != 0)
			{
				uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;
				s_WaterData.VertexBuffer.SetData(drawList.GetWaterVerticesPtr(), vertexCount * sizeof(Vertex_P3C2));
	
				RendererCommand::DrawElements(indexCount);

				Renderer::s_Stats.DrawCalls++;
				Renderer::s_Stats.Quads += vertexCount / quad_vertex_count;
			}
		}

		Renderer::s_Stats.WaterTimer.Finish();
	}

	void Renderer3D::RenderOutlinedBlock()
	{
		RendererCommand::DisableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::EnableLessEqualDepthTesting();

		s_OutlinedBlockData.Shader.      Bind();
		s_OutlinedBlockData.VertexArray. Bind();
		s_OutlinedBlockData.VertexBuffer.Bind();
		s_Data.QuadIndexBuffer.          Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), s_OutlinedBlockData.Position);
		s_OutlinedBlockData.Shader.SetMat4("u_Transform", transform);

		RendererCommand::DrawElements(cube_index_count);

		Renderer::s_Stats.DrawCalls++;
		Renderer::s_Stats.Quads += cube_face_cout;
	}

	void Renderer3D::RenderTinted()
	{
		RendererCommand::DisableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::DisableDepthTesting();

		s_TintedData.Shader.      Bind();
		s_TintedData.VertexArray. Bind();
		s_TintedData.VertexBuffer.Bind();

		glm::vec4 tintedColor = s_TintedData.Tinted ? s_TintedData.Color : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		s_TintedData.Shader.SetFloat4("u_Color", tintedColor);

		Texture2D::Bind(s_Data.RenderFrameBuffer.GetColorAttachmentRendererID(), default_texture_slot);
		RendererCommand::DrawArrays(quad_vertex_count_a);

		Renderer::s_Stats.DrawCalls++;
		Renderer::s_Stats.Quads ++;
	}

	void Renderer3D::RenderText()
	{
		RendererCommand::EnableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::DisableDepthTesting();

		s_TextData.Shader. Bind();
		s_TextData.Texture.Bind();

		s_TextData.VertexArray. Bind();
		s_TextData.VertexBuffer.Bind();

		Renderer3DUniformText* textBuffer = new Renderer3DUniformText[s_TextInfo.MaxCharacterUniformArrayLimit];

		uint32_t currentIndex = 0;
		for (const auto& [text, textStyle] : s_TextData.Data)
		{
			glm::vec2 currentPosition = textStyle.Position;

			float scale = textStyle.FontSize / s_TextInfo.FontTextureSize;

			glm::mat4 rotation = glm::translate(glm::mat4(1.0f), textStyle.Position) *
				glm::toMat4(glm::quat(textStyle.Rotation)) *
				glm::translate(glm::mat4(1.0f), -textStyle.Position);

			// adjust the height at which the text starts
			if (!text.empty())
			{
				const FontCharacter& character = s_TextData.Texture.GetCharacter(text[0]);
				currentPosition.y -= (character.Size.y) * scale * (1.0f + textStyle.FontSpacing);
			}

			for (auto c = text.begin(); c != text.end(); c++)
			{
				const FontCharacter& character = s_TextData.Texture.GetCharacter(*c);

				if (*c == '\n')
				{
					currentPosition.y -= (character.Size.y) * textStyle.FontSpacing * scale;
					currentPosition.x = textStyle.Position.x;
				}
				else if (*c == ' ')
				{
					currentPosition.x += (character.Advance >> 6) * scale;
				}
				else
				{
					float xpos = currentPosition.x + character.Bearing.x * scale;
					float ypos = currentPosition.y - (textStyle.FontSize - character.Bearing.y) * scale;

					textBuffer[currentIndex].Transform = rotation * glm::translate(glm::mat4(1.0f), { xpos, ypos, textStyle.Position.z }) *
						glm::scale(glm::mat4(1.0f), { s_TextInfo.FontTextureSize * scale, s_TextInfo.FontTextureSize * scale, 1.0f });

					textBuffer[currentIndex].Letter.x = (float)character.ID;

					textBuffer[currentIndex].Color = textStyle.Color;

					currentPosition.x += (character.Advance >> 6) * scale;
					currentIndex++;

					if (currentIndex == s_TextInfo.MaxCharacterUniformArrayLimit)
					{
						s_TextData.UniformBuffer.SetData(textBuffer, s_TextInfo.MaxCharacterUniformArrayLimit * sizeof(Renderer3DUniformText));
						RendererCommand::DrawStripArraysInstanced(4, currentIndex);
						currentIndex = 0;

						Renderer::s_Stats.DrawCalls++;
						Renderer::s_Stats.Quads += currentIndex;
					}
				}
			}
		}

		if (currentIndex)
		{
			s_TextData.UniformBuffer.SetData(textBuffer, s_TextInfo.MaxCharacterUniformArrayLimit * sizeof(Renderer3DUniformText));
			RendererCommand::DrawStripArraysInstanced(4, currentIndex);

			Renderer::s_Stats.DrawCalls++;
			Renderer::s_Stats.Quads += currentIndex;
		}

		delete[] textBuffer;
	}

	void Renderer3D::RenderQuads()
	{
		RendererCommand::EnableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::EnableLessEqualDepthTesting();

		s_QuadData.Shader.      Bind();
		s_QuadData.VertexArray. Bind();
		s_QuadData.VertexBuffer.Bind();
		s_Data.QuadIndexBuffer. Bind();

		s_QuadData.VertexOffset  = 0;
		uint32_t maxTextureSlots = Renderer::GetInfo().MaxTextureSlots;

		StartQuadsBatch();

		for (uint32_t i = 0; i < s_QuadData.Vertices.size(); i += quad_vertex_count)
		{
			if (s_QuadData.IndexCount == s_Info.MaxIndices)
				NextQuadsBatch();

			if (s_QuadData.Vertices[i].TexIndex == 0.0f) // just color, TexIndex temporarily holds the texture rendererID
			{
				s_QuadData.Vertices[i + 0].TexIndex = 0.0f;
				s_QuadData.Vertices[i + 1].TexIndex = 0.0f;
				s_QuadData.Vertices[i + 2].TexIndex = 0.0f;
				s_QuadData.Vertices[i + 3].TexIndex = 0.0f;

				s_QuadData.IndexCount += quad_index_count;
			}
			else // textures
			{	
				float textureIndex = 0.0f;
				for (uint32_t j = 1; j < s_QuadData.TextureSlotIndex; j++)
				{
					if (s_QuadData.TextureSlots[j] == s_QuadData.Vertices[i].TexIndex) // TexIndex temporarily holds the texture rendererID
					{
						textureIndex = (float)j;
						break;
					}
				}

				if (textureIndex == 0.0f)
				{
					if (s_QuadData.TextureSlotIndex >= maxTextureSlots)
						NextQuadsBatch();

					textureIndex = (float)s_QuadData.TextureSlotIndex;
					s_QuadData.TextureSlots[s_QuadData.TextureSlotIndex] = s_QuadData.Vertices[i].TexIndex;

					s_QuadData.TextureSlotIndex++;
				}

				s_QuadData.Vertices[i + 0].TexIndex = textureIndex;
				s_QuadData.Vertices[i + 1].TexIndex = textureIndex;
				s_QuadData.Vertices[i + 2].TexIndex = textureIndex;
				s_QuadData.Vertices[i + 3].TexIndex = textureIndex;

				s_QuadData.IndexCount += quad_index_count;
			}
		}

		FlushQuads();
	}

	void Renderer3D::FlushQuads()
	{
		if (s_QuadData.IndexCount == 0)
			return;

		uint32_t vertexCount = s_QuadData.IndexCount / quad_index_count * quad_vertex_count;

		s_QuadData.VertexBuffer.SetData(&s_QuadData.Vertices[s_QuadData.VertexOffset], vertexCount * sizeof(QuadVertex));
		s_QuadData.VertexOffset = vertexCount;

		for (uint32_t i = 0; i < s_QuadData.TextureSlotIndex; i++)
			Texture2D::Bind(s_QuadData.TextureSlots[i], i);
		
		RendererCommand::DrawElements(s_QuadData.IndexCount);

		Renderer::s_Stats.DrawCalls++;
		Renderer::s_Stats.Quads += vertexCount / quad_vertex_count;
	}

	void Renderer3D::StartQuadsBatch()
	{
		s_QuadData.IndexCount       = 0;
		s_QuadData.TextureSlotIndex = 1;
	}

	void Renderer3D::NextQuadsBatch()
	{
		FlushQuads();
		StartQuadsBatch();
	}

	void Renderer3D::RenderCubes()
	{
		RendererCommand::DisableBlending();
		RendererCommand::EnableFaceCulling();
		RendererCommand::EnableLessEqualDepthTesting();

		s_CubeData.Shader.      Bind();
		s_CubeData.VertexArray. Bind();
		s_CubeData.VertexBuffer.Bind();
		s_Data.QuadIndexBuffer. Bind();

		s_CubeData.VertexOffset  = 0;
		uint32_t maxTextureSlots = Renderer::GetInfo().MaxTextureSlots;

		StartCubesBatch();

		for (uint32_t i = 0; i < s_CubeData.Vertices.size(); i += quad_vertex_count)
		{
			if (s_CubeData.IndexCount == s_Info.MaxIndices)
				NextCubesBatch();

			if (s_CubeData.Vertices[i].TexIndex == 0.0f) // just color, TexIndex temporarily holds the texture rendererID
			{
				s_CubeData.Vertices[i + 0].TexIndex = 0.0f;
				s_CubeData.Vertices[i + 1].TexIndex = 0.0f;
				s_CubeData.Vertices[i + 2].TexIndex = 0.0f;
				s_CubeData.Vertices[i + 3].TexIndex = 0.0f;

				s_CubeData.IndexCount += quad_index_count;
			}
			else // textures
			{	
				float textureIndex = 0.0f;
				for (uint32_t j = 1; j < s_CubeData.TextureSlotIndex; j++)
				{
					if (s_CubeData.TextureSlots[j] == s_CubeData.Vertices[i].TexIndex) // TexIndex temporarily holds the texture rendererID
					{
						textureIndex = (float)j;
						break;
					}
				}

				if (textureIndex == 0.0f)
				{
					if (s_CubeData.TextureSlotIndex >= maxTextureSlots)
						NextCubesBatch();

					textureIndex = (float)s_CubeData.TextureSlotIndex;
					s_CubeData.TextureSlots[s_CubeData.TextureSlotIndex] = s_CubeData.Vertices[i].TexIndex;

					s_CubeData.TextureSlotIndex++;
				}

				s_CubeData.Vertices[i + 0].TexIndex = textureIndex;
				s_CubeData.Vertices[i + 1].TexIndex = textureIndex;
				s_CubeData.Vertices[i + 2].TexIndex = textureIndex;
				s_CubeData.Vertices[i + 3].TexIndex = textureIndex;

				s_CubeData.IndexCount += quad_index_count;
			}
		}

		FlushCubes();
	}

	void Renderer3D::FlushCubes()
	{
		if (s_CubeData.IndexCount == 0)
			return;

		uint32_t vertexCount = s_CubeData.IndexCount / quad_index_count * quad_vertex_count;

		s_CubeData.VertexBuffer.SetData(&s_CubeData.Vertices[s_CubeData.VertexOffset], vertexCount * sizeof(CubeVertex));
		s_CubeData.VertexOffset = vertexCount;

		for (uint32_t i = 0; i < s_CubeData.TextureSlotIndex; i++)
			Texture2D::Bind(s_CubeData.TextureSlots[i], i);
		
		RendererCommand::DrawElements(s_CubeData.IndexCount);

		Renderer::s_Stats.DrawCalls++;
		Renderer::s_Stats.Quads += vertexCount / quad_vertex_count;
	}

	void Renderer3D::StartCubesBatch()
	{
		s_CubeData.IndexCount = 0;
		s_CubeData.TextureSlotIndex = 1;
	}

	void Renderer3D::NextCubesBatch()
	{
		FlushCubes();
		StartCubesBatch();
	}

	void Renderer3D::PrepareQuads()
	{
		s_QuadData.VertexArray.Create();
		s_QuadData.VertexArray.Bind();

		s_QuadData.VertexBuffer.Create(s_Info.MaxVertices * sizeof(QuadVertex));
		s_QuadData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Float2, "a_TexCoor"  },
			{ ShaderDataType::Float,  "a_TexIndex" }
		});

		s_QuadData.VertexArray.SetVertexBuffer(s_QuadData.VertexBuffer);

		{
			TextureSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			s_QuadData.WhiteTexture.Create(spec);

			uint32_t white = 0xffffffff;
			s_QuadData.WhiteTexture.Setdata(&white, sizeof(uint32_t));
		}
		
		std::unordered_map<std::string, std::string> quadShaderData;
		uint32_t maxTextureSlots = Renderer::GetInfo().MaxTextureSlots;
		quadShaderData["#max_texture_slots"] = std::to_string(maxTextureSlots);

		s_QuadData.Shader.Create("assets/shaders/quad3D.vert.glsl", "assets/shaders/quad3D.frag.glsl", quadShaderData);
		s_QuadData.Shader.Bind();

		int* samplers = new int[maxTextureSlots];
		for (int i = 0; i < maxTextureSlots; i++)
			samplers[i] = i;
		s_QuadData.Shader.SetIntArray("u_Textures", samplers, maxTextureSlots);
		delete[] samplers;

		s_QuadData.TextureSlots    = new uint32_t[maxTextureSlots];
		s_QuadData.TextureSlots[0] = s_QuadData.WhiteTexture.GetRendererID();

		s_QuadData.Vertices.reserve(s_Info.MaxVertices);

		s_QuadData.VertexArray. Unbind();
		s_QuadData.VertexBuffer.Unbind();
		s_QuadData.Shader.      Unbind();
	}

	void Renderer3D::PrepareCubes()
	{
		s_CubeData.VertexArray.Create();
		s_CubeData.VertexArray.Bind();

		s_CubeData.VertexBuffer.Create(s_Info.MaxVertices * sizeof(QuadVertex));
		s_CubeData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"   },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Float2, "a_TexCoor"  },
			{ ShaderDataType::Float,  "a_TexIndex" }
		});

		s_CubeData.VertexArray.SetVertexBuffer(s_CubeData.VertexBuffer);

		{
			TextureSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			s_CubeData.WhiteTexture.Create(spec);

			uint32_t white = 0xffffffff;
			s_CubeData.WhiteTexture.Setdata(&white, sizeof(uint32_t));
		}
		
		std::unordered_map<std::string, std::string> cubeShaderData;
		uint32_t maxTextureSlots = Renderer::GetInfo().MaxTextureSlots;
		cubeShaderData["#max_texture_slots"] = std::to_string(maxTextureSlots);

		s_CubeData.Shader.Create("assets/shaders/cube.vert.glsl", "assets/shaders/cube.frag.glsl", cubeShaderData);
		s_CubeData.Shader.Bind();

		int* samplers = new int[maxTextureSlots];
		for (int i = 0; i < maxTextureSlots; i++)
			samplers[i] = i;
		s_CubeData.Shader.SetIntArray("u_Textures", samplers, maxTextureSlots);
		delete[] samplers;

		s_CubeData.TextureSlots    = new uint32_t[maxTextureSlots];
		s_CubeData.TextureSlots[0] = s_CubeData.WhiteTexture.GetRendererID();

		s_CubeData.Vertices.reserve(s_Info.MaxVertices);

		s_CubeData.VertexArray. Unbind();
		s_CubeData.VertexBuffer.Unbind();
		s_CubeData.Shader.      Unbind();
	}

	void Renderer3D::DrawChunk(Chunk* chunk)
	{
		s_ChunkData.Chunks.push_back(chunk);
	}

	void Renderer3D::DrawChunks(const std::vector<Chunk*>& chunks)
	{
		s_ChunkData.Chunks.insert(s_ChunkData.Chunks.end(), chunks.begin(), chunks.end());
	}

	void Renderer3D::DrawItem(const glm::vec3& position, const Item& block)
	{
		constexpr glm::vec3 block_initial_displacement{ 0.5f, 0.5f, 0.5f };
		glm::vec3 blockPosition = glm::vec3{ (int)position.x, (int)position.y, (int)position.z } + block_initial_displacement;

		DrawCube(blockPosition, { 0.0f, -glm::radians(90.0f * (float)block.Rotation), 0.0f }, { 1.0f, 1.0f, 1.0f }, AssetManager::GetItemTexture(block.Type));
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const glm::vec4& color)
	{
		glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotation));
		glm::mat4 transform      = glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), size);

		for (uint32_t i = 0; i < cube_vertex_count; i++)
		{
			CubeVertex vertex;
			vertex.Position = transform      * cube_vertices[i].Position;
			vertex.Normal   = rotationMatrix * cube_vertices[i].Normal;
			vertex.Color    = color;
			vertex.TexCoord = cube_vertices[i].UV;

			s_CubeData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotation));
		glm::mat4 transform      = glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), size);

		for (uint32_t i = 0; i < cube_vertex_count; i++)
		{
			CubeVertex vertex;
			vertex.Position = transform      * cube_vertices[i].Position;
			vertex.Normal   = rotationMatrix * cube_vertices[i].Normal;
			vertex.Color    = tintColor;
			vertex.TexCoord = cube_vertices[i].UV;
			vertex.TexIndex = (float)texture.GetRendererID();

			s_CubeData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer3D::DrawOutlinedBlock(const glm::vec3& position)
	{
		s_OutlinedBlockData.Status   = true;
		s_OutlinedBlockData.Position = position;
	}

	void Renderer3D::DrawWaterTinted()
	{
		s_TintedData.Tinted = true;
	}

	void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			QuadVertex vertex;
			vertex.Position = transform * quad_vertex_positions[i];
			vertex.Color    = color;
			vertex.TexCoord = quad_vertex_tex_coords[i];

			s_QuadData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			QuadVertex vertex;
			vertex.Position = transform * quad_vertex_positions[i];
			vertex.Color    = tintColor;
			vertex.TexCoord = quad_vertex_tex_coords[i] * tilingFactor;
			vertex.TexIndex = (float)texture.GetRendererID(); // TexIndex temporarily holds the texture rendererID

			s_QuadData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer3D::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float thickness)
	{
		glm::vec3 direction = end - start;
		glm::vec3 rotation  = glm::eulerAngles(glm::quatLookAt(glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f)));

		DrawCube(start + direction * 0.5f, rotation, glm::vec3(thickness, thickness, glm::length(direction)), color);
	}

	void Renderer3D::DrawText(const std::string& text, const TextStyle3D& textStyle)
	{
		s_TextData.Data.emplace_back(text, textStyle);
	}

	void Renderer3D::DrawText(const std::string& text, const glm::vec3& position, const glm::vec3& rotation)
	{
		DrawText(text, TextStyle3D{
			s_TextInfo.DefaultFontColor,
			position,
			rotation,
			s_TextInfo.DefaultFontSize,
			s_TextInfo.DefaultFontSpacing
		});
	}

	void Renderer3D::DrawText(const std::string& text, const glm::vec3& position, const glm::vec3& rotation, const glm::vec4& color)
	{
		DrawText(text, TextStyle3D{
			color,
			position,
			rotation,
			s_TextInfo.DefaultFontSize,
			s_TextInfo.DefaultFontSpacing
		});
	}

	void Renderer3D::DrawText(const std::string& text, const glm::vec3& position, const glm::vec3& rotation, float fontSize)
	{
		DrawText(text, TextStyle3D{
			s_TextInfo.DefaultFontColor,
			position,
			rotation,
			fontSize,
			s_TextInfo.DefaultFontSpacing
		});
	}

	void Renderer3D::DrawText(const std::string& text, const glm::vec3& position, const glm::vec3& rotation, const glm::vec4& color, float fontSize)
	{
		DrawText(text, TextStyle3D{
			color,
			position,
			rotation,
			fontSize,
			s_TextInfo.DefaultFontSpacing
		});
	}

}
