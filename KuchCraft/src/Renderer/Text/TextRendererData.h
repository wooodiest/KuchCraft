#pragma once

#include "Renderer/Data/Shader.h"
#include "Renderer/Data/IndexBuffer.h"
#include "Renderer/Data/VertexBuffer.h"
#include "Renderer/Data/VertexArray.h"
#include "Renderer/Data/UniformBuffer.h"
#include "Renderer/Data/FontTexture.h"

namespace KuchCraft {

	struct TextRendererInfo
	{
		std::string FontPath           = "assets/fonts/Roboto-Regular.ttf";
		glm::vec4   DefaultFontColor   = { 1.0f, 1.0f, 1.0f, 1.0f };
		float       DefaultFontSize    = 24.0f;
		float       DefaultFontSpacing = 1.3f;

		uint32_t MaxCharacterUniformArrayLimit = 400;
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

	struct UniformText
	{
		glm::mat4 Transform;
		glm::vec4 Letter{ 0.0f }; // weird alignment
	};

	struct TextRendererData
	{
		Shader        Shader;
		VertexArray   VertexArray;
		VertexBuffer  VertexBuffer;
		UniformBuffer UniformBuffer;
		FontTexture   Texture;

		std::vector<std::pair<std::string, TextStyle>> TextData;
	};

}