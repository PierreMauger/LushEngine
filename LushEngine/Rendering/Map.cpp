#include "Rendering/Map.hpp"

using namespace Lush;

MapMesh::MapMesh(int width, int height)
{
    this->_width = width;
    this->_height = height;

    std::vector<float> vertices;

    this->_rez = 20;
    for (unsigned int i = 0; i <= this->_rez - 1; i++) {
        for (unsigned int j = 0; j <= this->_rez - 1; j++) {
            vertices.push_back(-this->_width / 2.0f + this->_width * i / (float)this->_rez);   // v.x
            vertices.push_back(0.0f);                                                          // v.y
            vertices.push_back(-this->_height / 2.0f + this->_height * j / (float)this->_rez); // v.z
            vertices.push_back(i / (float)this->_rez);                                         // u
            vertices.push_back(j / (float)this->_rez);                                         // v

            vertices.push_back(-this->_width / 2.0f + this->_width * (i + 1) / (float)this->_rez); // v.x
            vertices.push_back(0.0f);                                                              // v.y
            vertices.push_back(-this->_height / 2.0f + this->_height * j / (float)this->_rez);     // v.z
            vertices.push_back((i + 1) / (float)this->_rez);                                       // u
            vertices.push_back(j / (float)this->_rez);                                             // v

            vertices.push_back(-this->_width / 2.0f + this->_width * i / (float)this->_rez);         // v.x
            vertices.push_back(0.0f);                                                                // v.y
            vertices.push_back(-this->_height / 2.0f + this->_height * (j + 1) / (float)this->_rez); // v.z
            vertices.push_back(i / (float)this->_rez);                                               // u
            vertices.push_back((j + 1) / (float)this->_rez);                                         // v

            vertices.push_back(-this->_width / 2.0f + this->_width * (i + 1) / (float)this->_rez);   // v.x
            vertices.push_back(0.0f);                                                                // v.y
            vertices.push_back(-this->_height / 2.0f + this->_height * (j + 1) / (float)this->_rez); // v.z
            vertices.push_back((i + 1) / (float)this->_rez);                                         // u
            vertices.push_back((j + 1) / (float)this->_rez);                                         // v
        }
    }
    glGenVertexArrays(1, &this->_VAO);
    glBindVertexArray(this->_VAO);

    glGenBuffers(1, &this->_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void MapMesh::draw()
{
    glBindVertexArray(this->_VAO);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_PATCHES, 0, this->_rez * this->_rez * 4);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
