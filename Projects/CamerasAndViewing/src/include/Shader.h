#pragma once

#include <string>
#include <unordered_map>

class Shader
{
private:
    unsigned int m_ShaderProgram;
    // Cache for uniform locations to improve performance
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

public:
    Shader(const char *vertexPath, const char *fragmentPath);

    // deletes shader program when a class goes out of scope
    ~Shader();

    // Activates the shader program
    void use() const;

    // The single, templated uniform setter to avoid multiple methods for GLSL Types
    template<typename T>
    void setUniform(const std::string &name, const T &value);

private:
    // Caches uniform location to avoid getting it every frame
    int getUniformLocation(const std::string &name) const;

    static void checkShaderCompileStatus(const unsigned int shader);

    static void checkShaderProgramStatus(const unsigned int program);
};
