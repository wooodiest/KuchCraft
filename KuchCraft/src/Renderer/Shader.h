#pragma once

#include <string>

namespace KuchCraft {

	class Shader
	{
	public:
		Shader();
		~Shader();

		void Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

		std::string ReadFile(const std::string& filepath);


		void Bind() const;
		void Unbind() const;

	private:
		unsigned int m_RendererID = 0;

	};

}