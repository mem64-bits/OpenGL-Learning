#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>


namespace core
{
    class Shader
    {
    private:
        unsigned int m_ShaderProgram;
        // Cache for uniform locations to improve performance
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;

    public:
        Shader(const char* vertexPath, const char* fragmentPath);

        // deletes shader program when a class goes out of scope
        ~Shader();

        // Activates the shader program
        void use() const;

        // The single, templated uniform setter to avoid multiple methods for GLSL Types
        template <typename T>
        void setUniform(const std::string& name, const T& value) const
        {
            int location = getUniformLocation(name);

            // Evaluates types at compile time to optimize away checks at runtime
            if constexpr(std::is_same_v<T, bool>)
            {
                glUniform1i(location, static_cast<int>( value ));
            } else if constexpr(std::is_same_v<T, int>)
            {
                glUniform1i(location, value);
            } else if constexpr(std::is_same_v<T, float>)
            {
                glUniform1f(location, value);
            } else if constexpr(std::is_same_v<T, glm::vec2>)
            {
                glUniform2fv(location, 1, glm::value_ptr(value));
            } else if constexpr(std::is_same_v<T, glm::vec3>)
            {
                glUniform3fv(location, 1, glm::value_ptr(value));
            } else if constexpr(std::is_same_v<T, glm::vec4>)
            {
                glUniform4fv(location, 1, glm::value_ptr(value));
            } else if constexpr(std::is_same_v<T, glm::mat3>)
            {
                glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
            } else if constexpr(std::is_same_v<T, glm::mat4>)
            {
                glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
            } else
            {
                // This will produce a compile-time error if you try to use an unsupported type.
                static_assert(sizeof(T) == 0, "Unsupported uniform type used in Shader::setUniform");
            }
        }

        // Caches uniform location to avoid getting it every frame
        int getUniformLocation(const std::string& name) const;

        void checkShaderCompileStatus(unsigned int shader) const;

        void checkShaderProgramStatus(unsigned int program) const;
    };
}
