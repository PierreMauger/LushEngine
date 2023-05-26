#include "Rendering/Texture.hpp"

using namespace Lush;

Texture::Texture(File &file)
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
    std::string content = file.load();
    unsigned char *data = stbi_load_from_memory((const stbi_uc *)content.c_str(), content.length(), &this->_width, &this->_height, &this->_nrChannels, 0);
    if (data) {
        this->_pixels.assign(data, data + this->_width * this->_height * this->_nrChannels);
        this->setupTexture();
        stbi_image_free(data);
    } else {
        stbi_image_free(data);
    }
}

unsigned int Texture::getId()
{
    return this->_id;
}

void Texture::setupTexture()
{
    glGenTextures(1, &this->_id);
    GLenum format = GL_RGB;
    if (this->_nrChannels == 1)
        format = GL_RED;
    else if (this->_nrChannels == 4)
        format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, this->_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, this->_width, this->_height, 0, format, GL_UNSIGNED_BYTE, this->_pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}
