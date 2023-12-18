#include "Rendering/Skybox.hpp"

using namespace Lush;

Skybox::Skybox(File &file) : Resource(ResourceType::SKYBOX, file)
{
    this->load(file);
}

// Skybox::~Skybox()
// {
//     // dtor is called when the Texture is inserted in the map
//     glDeleteTextures(1, &this->_id);
// }

void Skybox::load(const File &file)
{
    this->_content = file.load();
    this->createSkybox();
}

void Skybox::createSkybox()
{
    unsigned char *data = stbi_load_from_memory((const stbi_uc *)this->_content.c_str(), (int)this->_content.length(), &this->_width, &this->_height, &this->_nrChannels, 0);
    GLenum format = this->_nrChannels == 3 ? GL_RGB : GL_RGBA;
    static int faces[3][4] = {{0, 3, 0, 0}, {2, 5, 1, 6}, {0, 4, 0, 0}};

    if (!data)
        return;
    glGenTextures(1, &this->_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->_id);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, this->_width);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            if (faces[i][j] == 0)
                continue;
            glPixelStorei(GL_UNPACK_SKIP_PIXELS, j * this->_width / 4);
            glPixelStorei(GL_UNPACK_SKIP_ROWS, i * this->_height / 3);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faces[i][j] - 1, 0, format, this->_width / 4, this->_height / 3, 0, format, GL_UNSIGNED_BYTE, data);
        }
    }
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    stbi_image_free(data);
}

unsigned int Skybox::getId() const
{
    return this->_id;
}
