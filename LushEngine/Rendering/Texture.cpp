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

void Texture::load(const File &file)
{
    this->_content = file.load();
    this->createTexture();
}

void Texture::reload(const File &file)
{
    glDeleteTextures(1, &this->_id);
    this->load(file);
}

void Texture::createTexture()
{
    unsigned char *data = stbi_load_from_memory((const stbi_uc *)this->_content.c_str(), this->_content.length(), &this->_width, &this->_height, &this->_nrChannels, 0);
    GLenum format = this->_nrChannels == 1 ? GL_RED : this->_nrChannels == 3 ? GL_RGB : GL_RGBA;

    if (!data)
        return;
    if (this->_nrChannels == 4)
        this->_hasTransparency = Texture::calcTransparency(data, this->_width, this->_height);

    this->_heightData = new unsigned char[this->_width * this->_height];
    if (this->_nrChannels == 1) {
        memcpy(this->_heightData, data, this->_width * this->_height);
    } else if (this->_nrChannels == 3 || this->_nrChannels == 4) {
        for (int i = 0; i < this->_width * this->_height; ++i) {
            unsigned char r = data[i * this->_nrChannels];
            unsigned char g = data[i * this->_nrChannels + 1];
            unsigned char b = data[i * this->_nrChannels + 2];

            this->_heightData[i] = static_cast<unsigned char>(0.2989 * r + 0.587 * g + 0.114 * b);
        }
    }

    glGenTextures(1, &this->_id);
    glBindTexture(GL_TEXTURE_2D, this->_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, this->_width, this->_height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

bool Texture::calcTransparency(const unsigned char* data, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int alphaIndex = (y * width + x) * 4 + 3;
            if (data[alphaIndex] != 255 && data[alphaIndex] != 0) // Check if alpha is not fully opaque
                return true;
        }
    }
    return false;
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

unsigned char *Texture::getHeightData() const
{
    return this->_heightData;
}

std::string Texture::getContent() const
{
    return this->_content;
}

bool Texture::hasTransparency() const
{
    return this->_hasTransparency;
}
