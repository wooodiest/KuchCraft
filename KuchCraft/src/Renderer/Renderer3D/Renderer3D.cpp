#include "kcpch.h"
#include "Renderer/Renderer3D/Renderer3D.h"

#include "Core/Application.h"
#include "Core/Utils.h"

#include "Renderer/Renderer.h"
#include "Renderer/AssetManager.h"
#include "Renderer/RendererCommand.h"

#include "Renderer/Utils/FrustumCulling.h"

namespace KuchCraft {

	Renderer3DData            Renderer3D::s_Data;
	Renderer3DChunkData       Renderer3D::s_ChunkData;
	Renderer3DSkyboxData      Renderer3D::s_SkyboxData;
	Renderer3DWaterData       Renderer3D::s_WaterData;
	RendererOutlinedBlockData Renderer3D::s_OutlinedBlockData;

	void Renderer3D::Init()
	{
		KC_PROFILE_FUNCTION();

		LoadRenderer3DInfo();

		PrepareChunks();
		PrepareSkybox();
		PrepareWater();
		PrepareOutlinedBlock();

		// QuadIndexBuffer
		uint32_t* indices = new uint32_t[max_indices_in_chunk];
		uint32_t  offset = 0;
		for (uint32_t i = 0; i < max_indices_in_chunk; i += quad_index_count)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}
		s_Data.QuadIndexBuffer.Create(indices, max_indices_in_chunk);
		delete[] indices;

		FrameBufferSpecification frameBufferSpecification;
		frameBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		frameBufferSpecification.Width  = Application::Get().GetWindow().GetWidth();
		frameBufferSpecification.Height = Application::Get().GetWindow().GetHeight();
		s_Data.FrameBuffer.Create(frameBufferSpecification);
	}

	void Renderer3D::ShutDown()
	{
		KC_PROFILE_FUNCTION();
	}

	void Renderer3D::LoadRenderer3DInfo()
	{
		KC_PROFILE_FUNCTION();
	}

	void Renderer3D::Render()
	{
		KC_PROFILE_FUNCTION();

		s_Data.FrameBuffer.BindAndClear();

		FrustumCulling::Chunks(s_ChunkData.Chunks, *Renderer::s_Data.CurrentCamera, s_ChunkData.ChunksToRender);
		RenderChunks();
		RenderSkybox();

		if (s_OutlinedBlockData.Status)
			RenderOutlinedBlock();

		RenderWater();

		s_Data.FrameBuffer.Unbind();
	}

	void Renderer3D::Clear()
	{
		KC_PROFILE_FUNCTION();

		s_ChunkData.Chunks.        clear();
		s_ChunkData.ChunksToRender.clear();

		s_OutlinedBlockData.Status = false;
	}

	void Renderer3D::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		s_Data.FrameBuffer.Resize(width, height);
	}

	void Renderer3D::PrepareChunks()
	{
		KC_PROFILE_FUNCTION();

		s_ChunkData.VertexArray.Create();
		s_ChunkData.VertexArray.Bind();

		s_ChunkData.VertexBuffer.Create(max_vertices_in_chunk * sizeof(uint32_t));
		s_ChunkData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Uint, "a_PackedData" }
		});

		s_ChunkData.VertexArray.SetVertexBuffer(s_ChunkData.VertexBuffer);

		std::unordered_map<std::string, std::string> chunkShaderData;
		uint32_t maxTextureSlots = Renderer::GetInfo().MaxTextureSlots;
		chunkShaderData["##max_texture_slots"] = std::to_string(maxTextureSlots);

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
		KC_PROFILE_FUNCTION();

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
		KC_PROFILE_FUNCTION();

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
		KC_PROFILE_FUNCTION();
		
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
		outlinedBlockShaderData["##outlined_block_border_radius"] = std::to_string(s_OutlinedBlockData.BorderRadius);
		outlinedBlockShaderData["##outlined_block_border_color"]  = VecToString(s_OutlinedBlockData.Color);

		s_OutlinedBlockData.Shader.Create("assets/shaders/outlined_block.vert.glsl", "assets/shaders/outlined_block.frag.glsl", outlinedBlockShaderData);
		s_OutlinedBlockData.Shader.Bind();

		s_OutlinedBlockData.VertexArray .Unbind();
		s_OutlinedBlockData.VertexBuffer.Unbind();
		s_OutlinedBlockData.Shader      .Unbind();
	}

	void Renderer3D::RenderChunks()
	{
		KC_PROFILE_FUNCTION();

		RendererCommand::DisableBlending();
		RendererCommand::EnableFaceCulling();
		RendererCommand::EnableDepthTesting();

		s_ChunkData.Shader.Bind();
		s_ChunkData.VertexArray.Bind();
		s_Data.QuadIndexBuffer.Bind();

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
				}
			}
		}
	}

	void Renderer3D::RenderSkybox()
	{
		KC_PROFILE_FUNCTION();

		RendererCommand::DisableBlending();
		RendererCommand::EnableFrontFaceCulling();
		RendererCommand::EnableLessEqualDepthTesting();

		s_SkyboxData.Shader      .Bind();
		s_SkyboxData.VertexArray .Bind();
		s_SkyboxData.VertexBuffer.Bind();
		s_Data.QuadIndexBuffer   .Bind();

		s_SkyboxData.Texture.Bind();
		RendererCommand::DrawElements(cube_face_cout * quad_index_count);
	}

	void Renderer3D::RenderWater()
	{
		KC_PROFILE_FUNCTION();

		RendererCommand::EnableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::EnableDepthTesting();

		s_WaterData.Shader     .Bind();
		s_WaterData.VertexArray.Bind();
		s_Data.QuadIndexBuffer .Bind();

		AssetManager::GetBlockTexture(BlockType::Water).Bind(default_texture_slot);

		for (const auto& chunk : s_ChunkData.Chunks)
		{
			const auto& drawList = chunk->GetDrawList();
			uint32_t indexCount  = drawList.GetWaterVertices().size() / quad_vertex_count * quad_index_count;

			if (indexCount != 0)
			{
				uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;
				s_WaterData.VertexBuffer.SetData(drawList.GetWaterVerticesPtr(), vertexCount * sizeof(Vertex_P3C2));
	
				RendererCommand::DrawElements(indexCount);
			}
		}
	}

	void Renderer3D::RenderOutlinedBlock()
	{
		RendererCommand::DisableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::EnableLessEqualDepthTesting();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), s_OutlinedBlockData.Position);

		s_OutlinedBlockData.Shader.Bind();
		s_OutlinedBlockData.Shader.SetMat4("u_Transform", transform);

		s_OutlinedBlockData.VertexArray .Bind();
		s_OutlinedBlockData.VertexBuffer.Bind();
		s_Data.QuadIndexBuffer          .Bind();

		RendererCommand::DrawElements(cube_index_count);
	}

	void Renderer3D::DrawChunk(Chunk* chunk)
	{
		s_ChunkData.Chunks.push_back(chunk);
	}

	void Renderer3D::DrawChunks(const std::vector<Chunk*>& chunks)
	{
		s_ChunkData.Chunks.insert(s_ChunkData.Chunks.end(), chunks.begin(), chunks.end());
	}

	void Renderer3D::DrawOutlinedBlock(const glm::vec3& position)
	{
		s_OutlinedBlockData.Status   = true;
		s_OutlinedBlockData.Position = position;
	}

}
