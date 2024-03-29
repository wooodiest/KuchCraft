#pragma once

#include "Renderer/Shader.h"
#include "Renderer/OpenGL/IndexBuffer.h"
#include "Renderer/OpenGL/VertexBuffer.h"
#include "Renderer/OpenGL/VertexArray.h"

namespace KuchCraft {

	// Get this data to shader preprocess
	struct TextRendererInfo
	{
		std::string FontPath           = "assets/fonts/Roboto-Regular.ttf";
		glm::vec4   DefaultFontColor   = { 1.0f, 1.0f, 1.0f, 1.0f };
		float       DefaultFontSize    = 24.0f;
		float       DefaultFontSpacing = 1.3f;

		uint32_t MaxCharacterUniformArrayLimit = 400; // TODO: Add this in shader preprocess
		uint32_t FontTextureSize               = 256;
		uint32_t FontCharactersCount           = 128;
	};

	struct TextStyle
	{
		glm::vec4 Color;
		glm::vec2 Position;
		float     FontSize;
		float     FontSpacing;
	};

	struct TextCharacter
	{
		uint32_t   ID;
		uint32_t   Advance;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
	};

	struct UniformText
	{
		glm::mat4 Transform;
		glm::vec4 Letter{ 0.0f }; // weird alignment
	};

	constexpr uint32_t text_data_uniform_buffer_binding = 1; // TO REMOVE
	struct TextRendererData
	{
		Ref<Shader>       Shader;
		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		uint32_t          Texture       = 0;
		uint32_t          UniformBuffer = 0;

		std::map<char, TextCharacter> TextCharacters;
		std::vector<std::pair<std::string, TextStyle>> TextData;
	};

}