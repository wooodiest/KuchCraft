#pragma once

#include "Renderer/Data/Shader.h"
#include "Renderer/Data/IndexBuffer.h"
#include "Renderer/Data/VertexBuffer.h"
#include "Renderer/Data/VertexArray.h"
#include "Renderer/Data/UniformBuffer.h"
#include "Renderer/Data/FrameBuffer.h"
#include "Renderer/Data/Texture2D.h"
#include "Renderer/Data/FontTexture.h"

namespace KuchCraft {

	struct Renderer2DQuadInfo
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec2 Size     = { 100.0f,    100.f };
	};

	struct Renderer2DInfo
	{
		uint32_t MaxQuads    = 20000;
		uint32_t MaxVertices = MaxQuads * 4;
		uint32_t MaxIndices  = MaxQuads * 6;
	};

	struct Quad2DVertex
	{
		glm::vec3 Position { 0.0f, 0.0f, 0.0f       };
		glm::vec4 Color    { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 TexCoord { 0.0f, 0.0f             };
		float     TexIndex = 0.0f;
		int32_t   ID       = -1;
	};

	struct Quad3DVertex
	{
		glm::vec3 Position { 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal   { 0.0f, 0.0f, 1.0f };
		glm::vec4 Color    { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 TexCoord { 0.0f, 0.0f };
		float     TexIndex = 0.0f;
	};

	struct TransparentQuad3DVertex
	{
		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
		glm::vec2 TexCoord{ 0.0f, 0.0f };
		float     TexIndex = 0.0f;
	};

	struct Renderer2DID
	{
		int32_t ID;

		Renderer2DID()
			: ID(-1) {}

		Renderer2DID(int32_t id)
			: ID(id) {}

		operator int32_t() const { return ID; }
	};

	struct Renderer2DQuadData
	{
		FrameBuffer   FrameBuffer;
		Renderer2DID ID;

		Shader		 Shader;
		IndexBuffer  IndexBuffer;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;

		Texture2D WhiteTexture;

		std::vector<Quad2DVertex> Vertices;
		uint32_t* TextureSlots;
		uint32_t  TextureSlotIndex = 1;
		uint32_t  IndexCount   = 0;
		uint32_t  VertexOffset = 0;
	};

	struct Renderer2DFullScreenQuadData
	{
		Shader		 Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
	};

	struct TextStyle2D
	{
		glm::vec3 Position    { 0.0f, 0.0f, 0.0f       };
		glm::vec4 Color       { 1.0f, 1.0f, 1.0f, 1.0f };
		float     FontSize    = 24.0f;
		float     FontSpacing = 1.3f;
		bool      PositionFromTopLeft  = false;
	};

	struct Renderer2DUniformText
	{
		glm::mat4 Transform;
		glm::vec4 Color{ 1.0f };
		glm::vec4 Data{ 0.0f, -1.0f, 0.0f, 0.0f }; // Letter, ID
	};

	struct Renderer2DTextData
	{
		Shader        Shader;
		VertexArray   VertexArray;
		VertexBuffer  VertexBuffer;
		UniformBuffer UniformBuffer;
		FontTexture   Texture;

		std::vector<std::pair<uint32_t, float>> TextIndexDistance;
		std::vector<std::tuple<std::string, TextStyle2D, Renderer2DID>> Data;
	};

	struct Renderer2DTextInfo
	{
		std::string FontPath           = "assets/fonts/Roboto-Regular.ttf";
		glm::vec4   DefaultFontColor   = { 1.0f, 1.0f, 1.0f, 1.0f };

		uint32_t MaxCharacterUniformArrayLimit = 400;
		uint32_t FontTextureSize               = 256;
		uint32_t FontCharactersCount           = 128;
	};

	struct Renderer2DMouseData
	{
		bool Show = true;
		glm::vec2 Position = { 0.0f, 0.0f };

		glm::vec2 PrevPosition = { 0.0f, 0.0f };
	};

	constexpr glm::vec4 quad_vertex_positions[4] = {
		{ -1.0f, -1.0f, 0.0f, 1.0f },
		{  1.0f, -1.0f, 0.0f, 1.0f },
		{  1.0f,  1.0f, 0.0f, 1.0f },
		{ -1.0f,  1.0f, 0.0f, 1.0f }
	};

	constexpr glm::vec2 quad_vertex_tex_coords[4] = {
		{ 0.0f, 0.0f }, 
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};

}