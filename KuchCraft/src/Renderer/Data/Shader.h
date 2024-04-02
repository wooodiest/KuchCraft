#pragma once

namespace KuchCraft {

	class Shader
	{
	public:
		Shader();
		~Shader();

		void Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath,
			const std::unordered_map<std::string, std::string>& additionalPreProcessValues = std::unordered_map<std::string, std::string>());

		void SetMat4(const std::string& name, const glm::mat4& v);
		void SetInt(const std::string& name, int v);
		void SetFloat3(const std::string& name, const glm::vec3& v);
		void SetFloat4(const std::string& name, const glm::vec4& v);
		void SetIntArray(const std::string& name, int* values, uint32_t count);
		void SetFloat4Array(const std::string& name, float* data , uint32_t count);
	
		void Bind() const;
		void Unbind() const;

	private:
		std::string ReadFile(const std::string& filepath);
		void PreProcess(std::string& file, const std::unordered_map<std::string, std::string>& additionalPreProcessValues);
		uint32_t Compile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::unordered_map<std::string, std::string>& additionalPreProcessValues);

	private:
		uint32_t m_RendererID = 0;
	};

}