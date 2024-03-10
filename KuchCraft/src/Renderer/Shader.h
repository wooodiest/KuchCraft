#pragma once

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

namespace KuchCraft {

	class Shader
	{
	public:
		Shader();
		~Shader();

		void SetMat4(const std::string& name, const glm::mat4& v);
		void SetInt(const std::string& name, int v);
		void SetFloat4(const std::string& name, const glm::vec4& v);
		void SetIntArray(const std::string& name, int* values, uint32_t count);
		void SetFloat4Array(const std::string& name, float* data , uint32_t count);

		uint32_t Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
			
		void Bind() const;
		void Unbind() const;

	private:
		std::string ReadFile(const std::string& filepath);
		void PreProcess(std::string& file);

	private:
		uint32_t m_RendererID = 0;

	};

}