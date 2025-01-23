#pragma once
#include <string>
#include <unordered_map>
#include "../Vector3.h"

struct ShaderProgramSources
{
	std::string Vertex;
	std::string Fragment;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	// caching for uniforms
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filePath);
	~Shader();

	Shader() : m_RendererID(0) {} // remember to load shader later...
	void Load(const std::string& filePath);
	void Bind() const;
	void Unbind() const;

	// uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, Vector3 value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4(const std::string& name, Matrix4 mat4);
private:
	ShaderProgramSources ParseShader(std::string filepath);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int GetUniformLocation(const std::string& name);
};