#include "Mesh.hpp"

using namespace Lush;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material)
{
    this->_vertices = vertices;
    this->_indices = indices;
    this->_textures = textures;
    this->_material = material;
    this->setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &this->_VAO);
    glGenBuffers(1, &this->_VBO);
    glGenBuffers(1, &this->_EBO);

    glBindVertexArray(this->_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
    glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(Vertex), &this->_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(unsigned int), &this->_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, boneIDs));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, weights));
    glBindVertexArray(0);
}

void Mesh::draw(Shader &shader)
{
    shader.setBool("hasTexture", this->_textures.size() > 0);
    shader.setVec3("material.ambient", this->_material.ambient);
    shader.setVec3("material.diffuse", this->_material.diffuse);
    shader.setVec3("material.specular", this->_material.specular);
    shader.setFloat("material.shininess", this->_material.shininess);

    shader.setFloat("tex.shininess", 32.0f);
    for (unsigned int i = 0; i < this->_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        shader.setInt(this->_textures[i].type, i);
        glBindTexture(GL_TEXTURE_2D, this->_textures[i].id);
    }

    glBindVertexArray(this->_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(this->_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    if (this->_textures.size() > 0)
        glBindTexture(GL_TEXTURE_2D, 0);
}
