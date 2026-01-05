#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <type_traits>

// Simple trait to check if T is a std::vector
template <typename T>
struct is_vector : std::false_type {};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};

template <typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;


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

        template <typename T>
        void setUniform(const std::string& name, const T& value) const
        {
            int location = getUniformLocation(name);

            // checks if the type is vector
            if constexpr(is_vector_v<T>)
            {
                // V is the type inside the vector (e.g., float or glm::vec3)
                using V = T::value_type;
                const GLsizei count = static_cast<GLsizei>( value.size() );

                if constexpr(std::is_same_v<V, float>)
                    glUniform1fv(location, count, value.data());
                else if constexpr(std::is_same_v<V, int>)
                    glUniform1iv(location, count, value.data());
                else if constexpr(std::is_same_v<V, unsigned int>)// Support for GLSL 'uint'
                    glUniform1uiv(location, count, value.data());
                else if constexpr(std::is_same_v<V, glm::vec2>)
                    glUniform2fv(location, count, glm::value_ptr(value[0]));
                else if constexpr(std::is_same_v<V, glm::vec3>)
                    glUniform3fv(location, count, glm::value_ptr(value[0]));
                else if constexpr(std::is_same_v<V, glm::vec4>)
                    glUniform4fv(location, count, glm::value_ptr(value[0]));
                else if constexpr(std::is_same_v<V, glm::mat4>)
                    glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(value[0]));
            } else
            {
                if constexpr(std::is_same_v<T, bool> || std::is_same_v<T, int>)
                {
                    glUniform1i(location, static_cast<int>( value ));
                } else if constexpr(std::is_same_v<T, unsigned int>)// Single uint support
                {
                    glUniform1ui(location, value);
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
                } else if constexpr(std::is_same_v<T, glm::mat4>)
                {
                    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
                } else
                {
                    static_assert(sizeof(T) == 0, "Unsupported uniform type used in Shader::setUniform");
                }
            }
        }

        // Caches uniform location to avoid getting it every frame
        int getUniformLocation(const std::string& name) const;

        void checkShaderCompileStatus(unsigned int shader) const;

        void checkShaderProgramStatus(unsigned int program) const;
    };
}
