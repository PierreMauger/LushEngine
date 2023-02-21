#include "Rendering/CubeMap.hpp"

using namespace Lush;

CubeMap::CubeMap(std::vector<File> &files)
{
    this->load(files);
}

CubeMap::~CubeMap()
{
    // dtor is called when the Texture is inserted in the map
    glDeleteTextures(1, &this->_id);
}

void CubeMap::load(std::vector<File> &files)
{
    glGenTextures(1, &this->_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->_id);

    for (unsigned int i = 0; i < files.size(); i++) {
        std::string content = files[i].load();
        unsigned char *data = stbi_load_from_memory((const stbi_uc *)content.c_str(), content.length(), &this->_width, &this->_height, &this->_nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, this->_width, this->_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int CubeMap::getId()
{
    return this->_id;
}
