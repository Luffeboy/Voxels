#include "Shader.h"
#include "../IncludeOpenGL.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Shader::Shader(const std::string& filePath)
    : m_FilePath(filePath), m_RendererID(0)
{
    ShaderProgramSources source = ParseShader(filePath);
    if (source.Fragment.size() == 0 || source.Vertex.size() == 0)
    {
        std::cout << "shader at " << filePath << " could not be loaded" << std::endl;
    }
    m_RendererID = CreateShader(source.Vertex, source.Fragment);
}
void Shader::Load(const std::string& filePath)
{
    ShaderProgramSources source = ParseShader(filePath);
    if (source.Fragment.size() == 0 || source.Vertex.size() == 0)
    {
        std::cout << "shader at " << filePath << " could not be loaded" << std::endl;
    }
    m_RendererID = CreateShader(source.Vertex, source.Fragment);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // check for errors with the compiled shader

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // error handeling --

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // "iv" is for integer and vector
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Error compiling " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

ShaderProgramSources Shader::ParseShader(std::string filepath)
{
    std::ifstream stream(filepath);
    if (stream.bad())
    {
        std::cout << "Unable to parse shader " << filepath << std::endl;
    }
    enum ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1,
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            if (type == ShaderType::NONE) // no type currently defined
                continue;
            ss[(int)type] << line << '\n';
        }
    }
    stream.close();

    return { ss[0].str(), ss[1].str() };
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}
void Shader::Unbind() const
{
    glUseProgram(0);
}

// uniforms
void Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}
void Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}
void Shader::SetUniform3f(const std::string& name, Vector3 value)
{
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniformMat4(const std::string& name, Matrix4 mat4)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_TRUE, &mat4.values[0]);
}
unsigned int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning: uniform \"" << name << "\" doesn't exist" << std::endl;
    m_UniformLocationCache[name] = location;
    return location;
}
