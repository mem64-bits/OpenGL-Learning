#pragma once

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// For vecN uniform setters
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
    unsigned int shaderProgram;

public:
    Shader(const char *vertexPath, const char *fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ:: " << e.what() << std::endl;
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        // 2. compile shaders
        unsigned int vertexShader;
        unsigned int fragmentShader;

        // vertex Shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);
        checkShaderCompileStatus(vertexShader);

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);
        checkShaderCompileStatus(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        checkShaderProgramStatus(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    ~Shader()
    {
        glDeleteProgram(shaderProgram);
    }

    void use() const
    {
        glUseProgram(shaderProgram);
    }

    void checkShaderCompileStatus(unsigned int shader)
    {
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            int shaderType;
            std::string shaderName{"Unknown Shader"};
            glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);

            switch (shaderType)
            {
            case GL_VERTEX_SHADER:
                shaderName = "Vertex Shader";
                break;

            case GL_FRAGMENT_SHADER:
                shaderName = "Fragment Shader";
                break;
            default:
                return;
            }
            glGetShaderInfoLog(shader, 512, NULL, infoLog);

            std::cerr << shaderName << " failed to compile: " << infoLog << '\n';
        }
    }

    void checkShaderProgramStatus(unsigned int program)
    {
        int success;
        char infoLog[512];
        // after the program is linked we do a check to see if its valid
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cout << "Shader Program failed to link!: \n"
                      << infoLog << '\n';
        }
    }

    void setUniformInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }

    void setUniformBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), static_cast<int>(value));
    }
    void setUniformFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }

    void setUniformVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, glm::value_ptr(value));
    }

    void setUniformVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, glm::value_ptr(value));
    }

    void setUniformVec4(const std::string name, glm::vec4 value) const
    {
        glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, glm::value_ptr(value));
    }
};