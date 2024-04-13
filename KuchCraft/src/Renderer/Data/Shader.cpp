#include "kcpch.h"
#include "Renderer/Data/Shader.h"

#include "Renderer/Renderer.h"

#include <glad/glad.h>

namespace KuchCraft {

	Shader::Shader()
	{

	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::unordered_map<std::string, std::string>& additionalPreProcessValues)
	{
		Compile(vertexShaderPath, fragmentShaderPath, additionalPreProcessValues);
	}

	void Shader::SetMat4(const std::string& name, const glm::mat4& v)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
	}

	void Shader::SetInt(const std::string& name, int v)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, v);
	}

	void Shader::SetFloat3(const std::string& name, const glm::vec3& v)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, v.x, v.y, v.z);
	}

	void Shader::SetFloat4(const std::string& name, const glm::vec4& v)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, v.x, v.y, v.z, v.w);
	}

	void Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void Shader::SetFloat4Array(const std::string& name, float* data, uint32_t count)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, count, GL_FALSE, data);
	}

	uint32_t Shader::Compile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::unordered_map<std::string, std::string>& additionalPreProcessValues)
	{
		std::string vertex   = ReadFile(vertexShaderPath);
		std::string fragment = ReadFile(fragmentShaderPath);
		PreProcess(vertex, additionalPreProcessValues);
		PreProcess(fragment, additionalPreProcessValues);
		const char* vertex_cstr   = vertex.c_str();
		const char* fragment_cstr = fragment.c_str();

		int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertex_cstr, NULL);
		glCompileShader(vertexShader);
		{
			int success;
			char infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				KC_ERROR("Vertex shader compilation failed : {0} : {1}", vertexShaderPath.c_str(), infoLog);
			}
		}

		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragment_cstr, NULL);
		glCompileShader(fragmentShader);
		{
			int success;
			char infoLog[512];
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				KC_ERROR("Fragment shader compilation failed : {0} : {1}", fragmentShaderPath.c_str(), infoLog);
			}
		}

		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glLinkProgram(m_RendererID);

		{ 
			int success;
			char infoLog[512];
			glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
				KC_ERROR("Shader linking failed : {0}", infoLog);
			}
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return m_RendererID;
	}

	std::string Shader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				in.close();
			}
			else
			{
				KC_ERROR("Could not read shader from file : {0}", filepath.c_str());
			}
		}
		else
		{
			KC_ERROR("Could not open shader file: {0}", filepath.c_str());
		}

		return result;
	}

	void Shader::PreProcess(std::string& file, const std::unordered_map<std::string, std::string>& additionalPreProcessValues)
	{
		PreProcessIncludes(file);

		auto replace = [&](const std::string& from, const std::string& to) {
			size_t pos = 0;
			while ((pos = file.find(from, pos)) != std::string::npos)
			{
				file.replace(pos, from.length(), to);
				pos += to.length();
			}
		};

		std::for_each(additionalPreProcessValues.begin(), additionalPreProcessValues.end(), [&](const auto& pair) { replace(pair.first, pair.second); });
	}

	void Shader::PreProcessIncludes(std::string& file)
	{
		size_t pos = 0;
		const std::string includeDirective = "##include \"";
		while ((pos = file.find(includeDirective, pos)) != std::string::npos)
		{
			size_t endPos = file.find("\"", pos + includeDirective.length());
			if (endPos == std::string::npos)
			{
				KC_ERROR("Invalid include directive in shader : {0}", includeDirective.c_str());
				return;
			}

			std::string includePath = file.substr(pos + includeDirective.length(), endPos - (pos + includeDirective.length()));
			std::string includeContent = ReadFile(includePath);

			file.replace(pos, endPos + 1 - pos, includeContent);
			pos += includeContent.length();
		}
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

}