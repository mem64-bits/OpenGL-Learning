#pragma once

#include <glad/gl.h>
#include <Shader.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace core
{
    Shader::Shader(const char* vertexPath, const char* fragmentPath)
    {
        // retrieve the vertex/fragment source code from filePath
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
        } catch(const std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ:: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // compiles vertex shader from shader source code
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, & vShaderCode, nullptr);
        glCompileShader(vertexShader);
        checkShaderCompileStatus(vertexShader);

        // compiles fragment shader from shader source code
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, & fShaderCode, nullptr);
        glCompileShader(fragmentShader);
        checkShaderCompileStatus(fragmentShader);

        // creates shader program and attaches vertex and fragment shaders
        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vertexShader);
        glAttachShader(m_ShaderProgram, fragmentShader);
        glLinkProgram(m_ShaderProgram);
        checkShaderProgramStatus(m_ShaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // deletes shader program when a class goes out of scope
    Shader::~Shader()
    {
        glDeleteProgram(m_ShaderProgram);
    }

    // Activates the shader program
    void Shader::use() const
    {
        glUseProgram(m_ShaderProgram);
    }

    // Caches uniform location to avoid getting it every frame
    int Shader::getUniformLocation(const std::string& name) const
    {
        // Check if we already have this location in our cache
        if(m_UniformLocationCache.contains(name))
        {
            return m_UniformLocationCache[name];
        }

        // If not, retrieve it from OpenGL
        const int location = glGetUniformLocation(m_ShaderProgram, name.c_str());
        if(location == - 1)
        {
            std::cerr << "Warning: uniform '" << name << "' not found!" << std::endl;
        }

        // Add the location to our cache for future calls
        m_UniformLocationCache[name] = location;
        return location;
    }


    void Shader::checkShaderCompileStatus(const unsigned int shader)
    {
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, & success);

        if(!success)
        {
            int shaderType;
            std::string shaderName{ "Unknown Shader" };
            glGetShaderiv(shader, GL_SHADER_TYPE, & shaderType);

            switch(shaderType)
            {
                case GL_VERTEX_SHADER :
                    shaderName = "Vertex Shader";
                    break;

                case GL_FRAGMENT_SHADER :
                    shaderName = "Fragment Shader";
                    break;
                default :
                    break;
            }
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);

            std::cerr << shaderName << " failed to compile: " << infoLog << '\n';
        }
    }

    void Shader::checkShaderProgramStatus(const unsigned int program)
    {
        int success;
        char infoLog[512];
        // after the program is linked, we do a check to see if it's valid
        glGetProgramiv(program, GL_LINK_STATUS, & success);
        if(!success)
        {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "Shader Program failed to link!: \n"
                    << infoLog << '\n';
        }
    }
}
