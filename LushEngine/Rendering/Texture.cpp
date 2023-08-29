#include "Rendering/Texture.hpp"

using namespace Lush;

Texture::Texture(File &file) : Resource(ResourceType::TEXTURE, file)
{
    this->load(file);
}

Texture::~Texture()
{
    // dtor is called when the Texture is inserted in the map
    // glDeleteTextures(1, &_id);
}

void Texture::load(File &file)
{
    glGenTextures(1, &this->_id);
    this->_content = file.load();
    this->createTexture();
}

void Texture::createTexture()
{
    glGenTextures(1, &this->_id);
    unsigned char *data = stbi_load_from_memory((const stbi_uc *)this->_content.c_str(), this->_content.length(), &this->_width, &this->_height, &this->_nrChannels, 0);

    if (data) {
        this->_heightData = new unsigned char[this->_width * this->_height];
        if (this->_nrChannels == 1) {
            memcpy(this->_heightData, data, this->_width * this->_height);
        } else if (this->_nrChannels == 3 || this->_nrChannels == 4) {
            for (int i = 0; i < this->_width * this->_height; ++i) {
                unsigned char r = data[i * this->_nrChannels];
                unsigned char g = data[i * this->_nrChannels + 1];
                unsigned char b = data[i * this->_nrChannels + 2];
                unsigned char grayscale = static_cast<unsigned char>(0.2989 * r + 0.587 * g + 0.114 * b);

                this->_heightData[i] = grayscale;
            }
        }

        this->setupTexture(data);
        stbi_image_free(data);
    } else {
        stbi_image_free(data);
    }
}

void Texture::setupTexture(unsigned char *data)
{
    glGenTextures(1, &this->_id);
    GLenum format = GL_RGB;
    if (this->_nrChannels == 1)
        format = GL_RED;
    else if (this->_nrChannels == 4)
        format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, this->_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, this->_width, this->_height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::getId() const
{
    return this->_id;
}

int Texture::getWidth() const
{
    return this->_width;
}

int Texture::getHeight() const
{
    return this->_height;
}

unsigned char *Texture::getData() const
{
    return this->_heightData;
}
