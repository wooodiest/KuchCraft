#pragma once

#include "Renderer/Data/Shader.h"
#include "Renderer/Data/IndexBuffer.h"
#include "Renderer/Data/VertexBuffer.h"
#include "Renderer/Data/VertexArray.h"
#include "Renderer/Data/FrameBuffer.h"
#include "Renderer/Data/CubeMapTexture.h"
#include "Renderer/Renderer2D/Renderer2DData.h"
#include "Renderer/RendererData.h"
#include "World/Chunk.h"

namespace KuchCraft {

	struct Renderer3DInfo
	{
		uint32_t MaxQuads    = 20000;
		uint32_t MaxVertices = MaxQuads * 4;
		uint32_t MaxIndices  = MaxQuads * 6;
	};

	struct Renderer3DData
	{
		IndexBuffer QuadIndexBuffer;
		FrameBuffer RenderFrameBuffer;
	};

	struct Renderer3DChunkData
	{
		Shader       Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;

		std::vector<Chunk*> Chunks;
		std::vector<Chunk*> ChunksToRender;
	};

	struct Renderer3DSkyboxData
	{
		Shader         Shader;
		VertexArray    VertexArray;
		VertexBuffer   VertexBuffer;
		CubeMapTexture Texture;
	};

	struct Renderer3DOutlinedCubeData
	{
		bool Status = false;
		glm::vec3 Position{ 0.0f };
		glm::vec3 Size    { 0.5f, 0.5f, 0.5f };
		glm::vec4 Color   { 1.0f, 1.0f, 1.0f, 1.0f };
		float BorderRadius = 0.005f;

		Shader       Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
	};

	struct Renderer3DTintedData
	{
		bool Tinted = false;
		glm::vec4 Color{ 0.0f, 0.0f, 0.75f, 1.0f };

		Shader       Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
		FrameBuffer  FrameBuffer;
	};

	struct Renderer3DQuadData
	{
		Shader		 Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;

		Texture2D WhiteTexture;

		std::vector<Quad3DVertex> Vertices;
		uint32_t* TextureSlots;
		uint32_t  TextureSlotIndex = 1;
		uint32_t  IndexCount       = 0;
		uint32_t  VertexOffset     = 0;
	};

	struct Renderer3DTransparentQuadData
	{
		Shader		 Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
		IndexBuffer  IndexBuffer;

		Texture2D WhiteTexture;

		std::vector<TransparentQuad3DVertex> Vertices;

		uint32_t* TextureSlots;
		uint32_t  TextureSlotIndex = 1;
		uint32_t  IndexCount       = 0;
		uint32_t  VertexOffset     = 0;
	};

	struct CubeVertex
	{
		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal  { 0.0f, 0.0f, 0.0f };
		glm::vec4 Color   { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 TexCoord{ 0.0f, 0.0f };
		float     TexIndex = 0.0f;
	};

	struct Renderer3DCubeData
	{
		Shader		 Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;

		Texture2D WhiteTexture;

		std::vector<CubeVertex> Vertices;
		uint32_t* TextureSlots;
		uint32_t  TextureSlotIndex = 1;
		uint32_t  IndexCount       = 0;
		uint32_t  VertexOffset     = 0;
	};

	struct TextStyle3D
	{
		glm::vec4 Color    = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		float     FontSize    = 1.0f;
		float     FontSpacing = 1.3f;
	};

	struct Renderer3DUniformText
	{
		glm::mat4 Transform;
		glm::vec4 Color{ 1.0f };
		glm::vec4 Letter{ 0.0f }; // weird alignment
	};

	struct Renderer3DTextData
	{
		Shader        Shader;
		VertexArray   VertexArray;
		VertexBuffer  VertexBuffer;
		UniformBuffer UniformBuffer;
		FontTexture   Texture;

		std::vector<std::pair<std::string, TextStyle3D>> Data;
	};

	struct Renderer3DTextInfo
	{
		std::string FontPath           = "assets/fonts/Roboto-Regular.ttf";
		glm::vec4   DefaultFontColor   = { 1.0f, 1.0f, 1.0f, 1.0f };

		uint32_t MaxCharacterUniformArrayLimit = 400;
		uint32_t FontTextureSize               = 512;
		uint32_t FontCharactersCount           = 128;
	};

	struct CubeVertexHelper
	{
		glm::vec4 Position{ 0.0f, 0.0f, 0.0f, 1.0f };
		glm::vec4 Normal  { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec2 UV      { 0.0f, 0.0f };
	};

	constexpr CubeVertexHelper cube_vertices[] = {
		/* bottom */
		CubeVertexHelper{ 
			glm::vec4(0.5f, -0.5f, 0.5f, 1.0f),
			glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
			glm::vec2(0.25f, 0.5f)
		},
		CubeVertexHelper{
			glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f), 
			glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
			glm::vec2(0.0f, 0.5f) 
		},
		CubeVertexHelper{ 
			glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f),
			glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
			glm::vec2(0.0f, 0.0f)
		},
		CubeVertexHelper{ 
			glm::vec4(0.5f, -0.5f, -0.5f, 1.0f),
			glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
			glm::vec2(0.25f, 0.0f)
		},

		/* top */
		CubeVertexHelper{ 
			glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),
			glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec2(0.25f, 0.0f) 
		},
		CubeVertexHelper{ 
			glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
			glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec2(0.5f, 0.0f) 
		},
		CubeVertexHelper{ 
			glm::vec4(0.5f, 0.5f, -0.5f, 1.0f),
			glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec2(0.5f, 0.5f) 
		},
		CubeVertexHelper{ 
			glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f),
			glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), 
			glm::vec2(0.25f, 0.5f) 
		},

		/* front */
		CubeVertexHelper{ 
			glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
			glm::vec2(0.0f, 0.5f) 
		},
		CubeVertexHelper{ 
			glm::vec4(0.5f, -0.5f, 0.5f, 1.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
			glm::vec2(0.25f, 0.5f) 
		},
		CubeVertexHelper{ 
			glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
			glm::vec2(0.25f, 1.0f) 
		},
		CubeVertexHelper{ 
			glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
			glm::vec2(0.0f, 1.0f)
		},

		/* right */
		CubeVertexHelper{
			glm::vec4(0.5f, -0.5f, 0.5f, 1.0f), 
			glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec2(0.25f, 0.5f) 
		},
		CubeVertexHelper{
			glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), 
			glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), 
			glm::vec2(0.5f, 0.5f) 
		},
		CubeVertexHelper{
			glm::vec4(0.5f, 0.5f, -0.5f, 1.0f),
			glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec2(0.5f, 1.0f)
		},
		CubeVertexHelper{
			glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
			glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec2(0.25f, 1.0f) 
		},

		/* behind */
		CubeVertexHelper{
			glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), 
			glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
			glm::vec2(0.5f, 0.5f) 
		},
		CubeVertexHelper{
			glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f),
			glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
			glm::vec2(0.75f, 0.5f) 
		},
		CubeVertexHelper{
			glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f),
			glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
			glm::vec2(0.75f, 1.0f) 
		},
		CubeVertexHelper{ 
			glm::vec4(0.5f, 0.5f, -0.5f, 1.0f),
			glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
			glm::vec2(0.5f, 1.0f) 
		},

		/* left */
		CubeVertexHelper{ 
			glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f),
			glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec2(0.75f, 0.5f)
		 },
		CubeVertexHelper{
			 glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f),
			 glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
			 glm::vec2(1.0f, 0.5f) 
		},
		CubeVertexHelper{
			glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),
			glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f), 
			glm::vec2(1.0f, 1.0f) 
		},
		CubeVertexHelper{
			glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f),
			glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f), 
			glm::vec2(0.75f, 1.0f) 
		},

	};

	constexpr Vertex_P3C2 outlined_cube_vertices[cube_vertex_count] = {
		Vertex_P3C2{glm::vec3{  1.0f, -1.0f,  1.0f }, glm::vec2{ 1.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec2{ 0.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec2{ 0.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{  1.0f, -1.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec2{ 1.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{  1.0f,  1.0f,  1.0f }, glm::vec2{ 0.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{  1.0f,  1.0f, -1.0f }, glm::vec2{ 0.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec2{ 1.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{  1.0f, -1.0f,  1.0f }, glm::vec2{ 0.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{  1.0f,  1.0f,  1.0f }, glm::vec2{ 0.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec2{ 1.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{  1.0f, -1.0f,  1.0f }, glm::vec2{ 1.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{  1.0f, -1.0f, -1.0f }, glm::vec2{ 0.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{  1.0f,  1.0f, -1.0f }, glm::vec2{ 0.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{  1.0f,  1.0f,  1.0f }, glm::vec2{ 1.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{  1.0f, -1.0f, -1.0f }, glm::vec2{ 1.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec2{ 0.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec2{ 0.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{  1.0f,  1.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec2{ 1.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec2{ 0.0f, 0.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec2{ 0.0f, 1.0f }},
		Vertex_P3C2{glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f }}
	};

}