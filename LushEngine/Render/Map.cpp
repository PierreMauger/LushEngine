#include "Map.hpp"

using namespace Lush;

Map::Map(unsigned char *map, int width, int height, int nrChannels)
{
    this->_width = width;
    this->_height = height;
    this->_nrChannels = nrChannels;

    std::vector<float> vertices;
    float yScale = 64.0f / 256.0f, yShift = 32.0f;
    int rez = 1;
    for (int i = 0; i < this->_height; i++) {
        for (int j = 0; j < this->_width; j++) {
            unsigned char *pixelOffset = map + (j + this->_width * i) * this->_nrChannels;
            unsigned char y = pixelOffset[0];

            vertices.push_back(-this->_height / 2.0f + this->_height * i / (float)this->_height); // vx
            vertices.push_back((int)y * yScale - yShift);                                         // vy
            vertices.push_back(-this->_width / 2.0f + this->_width * j / (float)this->_width);    // vz
        }
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i < this->_height - 1; i += rez)
        for (int j = 0; j < this->_width; j += rez)
            for (int k = 0; k < 2; k++)
                indices.push_back(j + this->_width * (i + k * rez));

    this->_numStrips = (this->_height - 1) / rez;
    this->_numTrisPerStrip = (this->_width / rez) * 2 - 2;

    glGenVertexArrays(1, &this->_VAO);
    glBindVertexArray(this->_VAO);
    glGenBuffers(1, &this->_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &this->_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
}

void Map::draw([[maybe_unused]] Shader &shader)
{
    glBindVertexArray(this->_VAO);
    glCullFace(GL_FRONT);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int strip = 0; strip < this->_numStrips; strip++)
        glDrawElements(GL_TRIANGLE_STRIP, this->_numTrisPerStrip + 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * (this->_numTrisPerStrip + 2) * strip));
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glCullFace(GL_BACK);
}
