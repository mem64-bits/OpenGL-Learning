#pragma once
#include <glad/gl.h>
#include "ImageLoader.h"
#include <string>


namespace core
{
    struct TextureParameters
    {
        unsigned int wrapS = GL_REPEAT;
        unsigned int wrapT = GL_REPEAT;
        unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR;
        unsigned int magFilter = GL_LINEAR;
    };

    class Texture
    {
        std::string m_Filepath;
        unsigned int m_TextureID;
        int m_Width;
        int m_Height;

    public:
        // Deletes copy constructor (no texture object can delete same id)
        Texture& operator=(const Texture&) = delete;

        Texture(const Texture&) = delete;

        ~Texture();

        explicit Texture(const std::string& path, const TextureParameters& params = {});

        void bindTexture(const unsigned int slot = 0) const;

        int getWidth() const;

        int getHeight() const;
    };
}
