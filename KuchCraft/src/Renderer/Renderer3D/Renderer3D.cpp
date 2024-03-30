#include "kcpch.h"
#include "Renderer/Renderer3D/Renderer3D.h"

#include "Core/Application.h"

#include "Renderer/Renderer.h"
#include "Renderer/AssetManager.h"

#include <glad/glad.h>
#include <stb_image.h> // tmp

namespace KuchCraft {

	Renderer3DData            Renderer3D::s_Data;
	Renderer3DChunkData       Renderer3D::s_ChunkData;
	Renderer3DSkyboxData      Renderer3D::s_SkyboxData;
	Renderer3DWaterData       Renderer3D::s_WaterData;
	RendererOutlinedBlockData Renderer3D::s_OutlinedBlockData;

	void Renderer3D::Init()
	{
		KC_PROFILE_FUNCTION();

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
		s_Data.QuadIndexBuffer = IndexBuffer::Create(indices, max_indices_in_chunk);
		delete[] indices;

		FrameBufferSpecification frameBufferSpecification;
		frameBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		frameBufferSpecification.Width  = Application::Get().GetWindow().GetWidth();
		frameBufferSpecification.Height = Application::Get().GetWindow().GetHeight();
		s_Data.MainFrameBuffer = FrameBuffer::Create(frameBufferSpecification);
	}

	void Renderer3D::ShutDown()
	{
		KC_PROFILE_FUNCTION();
	}

	void Renderer3D::Render()
	{
		KC_PROFILE_FUNCTION();

		s_Data.MainFrameBuffer->Bind();
		s_Data.MainFrameBuffer->Clear();

		RenderChunks();
		RenderSkybox();

		if (s_OutlinedBlockData.Status)
			RenderOutlinedBlock();

		RenderWater();

		s_Data.MainFrameBuffer->Unbind();
	}

	void Renderer3D::Clear()
	{
		KC_PROFILE_FUNCTION();

		s_ChunkData.Chunks.clear();
		s_OutlinedBlockData.Status = false;
	}

	void Renderer3D::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		s_Data.MainFrameBuffer->Resize(width, height);
	}

	void Renderer3D::PrepareChunks()
	{
		KC_PROFILE_FUNCTION();

		s_ChunkData.VertexArray = VertexArray::Create();
		s_ChunkData.VertexArray->Bind();

		s_ChunkData.VertexBuffer = VertexBuffer::Create(max_vertices_in_chunk * sizeof(uint32_t));
		s_ChunkData.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Uint, "a_PackedData" }
		});

		s_ChunkData.VertexArray->SetVertexBuffer(s_ChunkData.VertexBuffer);

		s_ChunkData.Shader = Shader::Create("assets/shaders/chunk.vert.glsl", "assets/shaders/chunk.frag.glsl");
		s_ChunkData.Shader->Bind();

		int samplers[max_texture_slots];
		for (int i = 0; i < max_texture_slots; i++)
			samplers[i] = i;
		s_ChunkData.Shader->SetIntArray("u_Textures", samplers, max_texture_slots);

		s_ChunkData.VertexArray ->Unbind();
		s_ChunkData.VertexBuffer->Unbind();
		s_ChunkData.Shader      ->Unbind();
	}

	void Renderer3D::PrepareSkybox()
	{
		KC_PROFILE_FUNCTION();

		s_SkyboxData.VertexArray = VertexArray::Create();
		s_SkyboxData.VertexArray->Bind();

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

		s_SkyboxData.VertexBuffer = VertexBuffer::Create(sizeof(vertices), vertices, true);
		s_SkyboxData.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position"}
		});

		s_SkyboxData.VertexArray->SetVertexBuffer(s_SkyboxData.VertexBuffer);

		s_SkyboxData.Shader = Shader::Create("assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
		s_SkyboxData.Shader->Bind();
		s_SkyboxData.Shader->SetInt("u_CubemapTexture", 0);

		{ // tmp
			const GLenum types[cube_face_cout] = {
				GL_TEXTURE_CUBE_MAP_POSITIVE_X,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
			};
			const std::string paths[cube_face_cout] =
			{
				"assets/skybox/xpos.png",
				"assets/skybox/xneg.png",
				"assets/skybox/ypos.png",
				"assets/skybox/yneg.png",
				"assets/skybox/zpos.png",
				"assets/skybox/zneg.png"
			};
			uint32_t texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

			for (uint32_t i = 0; i < cube_face_cout; i++)
			{
				int width, height, channels;
				stbi_set_flip_vertically_on_load(0);
				unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &channels, 4);

				glTexImage2D(types[i], 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				stbi_image_free(data);
			}
			s_SkyboxData.Texture = texture;
		}

		s_SkyboxData.VertexArray ->Unbind();
		s_SkyboxData.VertexBuffer->Unbind();
		s_SkyboxData.Shader      ->Unbind();
	}

	void Renderer3D::PrepareWater()
	{
		KC_PROFILE_FUNCTION();

		s_WaterData.VertexArray = VertexArray::Create();
		s_WaterData.VertexArray->Bind();

		s_WaterData.VertexBuffer = VertexBuffer::Create(max_vertices_in_chunk * sizeof(Vertex_P3C2));
		s_WaterData.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_WaterData.VertexArray->SetVertexBuffer(s_WaterData.VertexBuffer);

		s_WaterData.Shader = Shader::Create("assets/shaders/water.vert.glsl", "assets/shaders/water.frag.glsl");
		s_WaterData.Shader->Bind();
		s_WaterData.Shader->SetInt("u_Texture", default_texture_slot);

		s_WaterData.VertexArray ->Unbind();
		s_WaterData.VertexBuffer->Unbind();
		s_WaterData.Shader      ->Unbind();
	}

	void Renderer3D::PrepareOutlinedBlock()
	{
		KC_PROFILE_FUNCTION();
		
		s_OutlinedBlockData.VertexArray = VertexArray::Create();
		s_OutlinedBlockData.VertexArray->Bind();

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

		s_OutlinedBlockData.VertexBuffer = VertexBuffer::Create(sizeof(vertices), vertices, true);
		s_OutlinedBlockData.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_OutlinedBlockData.VertexArray->SetVertexBuffer(s_OutlinedBlockData.VertexBuffer);

		s_OutlinedBlockData.Shader = Shader::Create("assets/shaders/outlined_block.vert.glsl", "assets/shaders/outlined_block.frag.glsl");
		s_OutlinedBlockData.Shader->Bind();

		s_OutlinedBlockData.VertexArray ->Unbind();
		s_OutlinedBlockData.VertexBuffer->Unbind();
		s_OutlinedBlockData.Shader      ->Unbind();
	}

	void Renderer3D::RenderChunks()
	{
		KC_PROFILE_FUNCTION();

		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		s_ChunkData.Shader->Bind();
		s_ChunkData.VertexArray->Bind();

		for (const auto& chunk : s_ChunkData.Chunks)
		{
			uint32_t vertexOffset = 0;
			const auto& drawList = chunk->GetDrawList();
			s_ChunkData.Shader->SetFloat3("u_ChunkPosition", chunk->GetPosition());

			for (uint32_t i = 0; i < drawList.GetDrawCallCount(); i++)
			{
				uint32_t indexCount = drawList.GetIndexCount(i);
				if (indexCount != 0)
				{
					uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;
					s_ChunkData.VertexBuffer->SetData(drawList.GetVerticesPtr(vertexOffset), vertexCount * sizeof(uint32_t));

					vertexOffset += vertexCount;

					for (uint32_t j = 0; j < drawList.GetTextureCount(i); j++)
						Texture2D::Bind(drawList.GetTexture(i, j), j);

					s_Data.QuadIndexBuffer->Bind();
					glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
				}
			}
		}
	}

	void Renderer3D::RenderSkybox()
	{
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);

		s_SkyboxData.Shader      ->Bind();
		s_SkyboxData.VertexArray ->Bind();
		s_SkyboxData.VertexBuffer->Bind();
		s_Data.QuadIndexBuffer   ->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_SkyboxData.Texture);
		glDrawElements(GL_TRIANGLES, cube_face_cout * quad_index_count, GL_UNSIGNED_INT, nullptr);

		glCullFace(GL_BACK);
		glDepthFunc(GL_LESS);
	}

	void Renderer3D::RenderWater()
	{
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);

		s_WaterData.Shader->Bind();
		s_WaterData.VertexArray->Bind();

		for (const auto& chunk : s_ChunkData.Chunks)
		{
			const auto& drawList = chunk->GetDrawList();
			uint32_t indexCount  = drawList.GetWaterVertices().size() / quad_vertex_count * quad_index_count;

			if (indexCount != 0)
			{
				uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;
				s_WaterData.VertexBuffer->SetData(drawList.GetWaterVerticesPtr(), vertexCount * sizeof(Vertex_P3C2));

				AssetManager::GetBlockTexture(BlockType::Water)->Bind(default_texture_slot);
				// Draw elements		
				s_Data.QuadIndexBuffer->Bind();
				glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
			}
		}
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}

	void Renderer3D::RenderOutlinedBlock()
	{
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), s_OutlinedBlockData.Position);

		s_OutlinedBlockData.Shader->Bind();
		s_OutlinedBlockData.Shader->SetMat4("u_Transform", transform);

		s_OutlinedBlockData.VertexArray->Bind();
		s_OutlinedBlockData.VertexBuffer->Bind();

		s_Data.QuadIndexBuffer->Bind();
		glDrawElements(GL_TRIANGLES, cube_index_count, GL_UNSIGNED_INT, nullptr);

		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
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
