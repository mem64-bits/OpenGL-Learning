#pragma once
#include <filesystem>
#include <string>


class ImageLoader
{
    std::string m_filepath;
    int m_width;
    int m_height;
    int m_nrChannels;
    unsigned char *m_data = nullptr;

public:
    ImageLoader() = default;

    ImageLoader(const ImageLoader &) = delete;

    ImageLoader &operator=(const ImageLoader &) = delete;

    explicit ImageLoader(const std::string &filepath);

    ~ImageLoader();

    bool loadImage(const std::string &filepath);

    void unloadImage();

    bool imageLoaded() const;

    unsigned char *getImage() const;

    std::string getFilepath() const;

    int getWidth() const;

    int getHeight() const;

    int getNrChannels() const;
};
