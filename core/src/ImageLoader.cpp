#include <ImageLoader.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <stb_image.h>

namespace core
{
    namespace fs = std::filesystem;


    ImageLoader::ImageLoader(const std::string& filepath)
    {
        m_filepath = filepath;
        loadImage(m_filepath);
    }

    ImageLoader::~ImageLoader()
    {
        unloadImage();
    }

    bool ImageLoader::loadImage(const std::string& filepath)
    {
        unloadImage();

        if(!fs::exists(filepath) || !fs::is_regular_file(filepath))
        {
            std::cerr << "(ERROR) ImageLoader: File does not exist : " << filepath << std::endl;
            return false;
        }
        m_filepath = filepath;

        stbi_set_flip_vertically_on_load(true);
        m_data = stbi_load(m_filepath.c_str(), & m_width, & m_height, & m_nrChannels, 0);

        if(m_data == nullptr)
        {
            std::cerr << "Failed to load image at: " << m_filepath << std::endl;
            m_filepath.clear();
            return false;
        }

        return true;
    }


    void ImageLoader::unloadImage()
    {
        stbi_image_free(m_data);
        m_height = 0;
        m_width = 0;
        m_nrChannels = 0;
    }

    bool ImageLoader::imageLoaded() const { return m_data != nullptr; }
    unsigned char* ImageLoader::getImage() const { return m_data; }
    std::string ImageLoader::getFilepath() const { return m_filepath; }

    int ImageLoader::getWidth() const { return m_width; }
    int ImageLoader::getHeight() const { return m_height; }
    int ImageLoader::getNrChannels() const { return m_nrChannels; }
}
