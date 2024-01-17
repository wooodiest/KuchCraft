#pragma once

#include <string>
#include <glm/glm.hpp>

namespace KuchCraft {

	class Shader
	{
	public:
		Shader();
		~Shader();

		void SetMat4(const std::string& name, const glm::mat4& v);

		void Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
			
		std::string ReadFile(const std::string& filepath);

		void Bind() const;
		void Unbind() const;

	private:
		unsigned int m_RendererID = 0;

	};

}