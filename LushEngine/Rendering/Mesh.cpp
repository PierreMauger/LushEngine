#include "Rendering/Mesh.hpp"

using namespace Lush;

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Tex> &textures, std::string material)
{
    this->_vertices = vertices;
    this->_indices = indices;
    this->_textures = textures;
    this->_material = material;
    this->setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &this->_bufferObject.vao);
    glGenBuffers(1, &this->_bufferObject.vbo);
    glGenBuffers(1, &this->_bufferObject.ebo);

    glBindVertexArray(this->_bufferObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->_bufferObject.vbo);
    glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(Vertex), &this->_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_bufferObject.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(unsigned int), &this->_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
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

void Mesh::draw(Shader &shader, Model &model, std::unordered_map<std::string, std::unique_ptr<Texture>> &textures)
{
    Material material;
    if (model.materials.contains(this->_material))
        material = model.materials[this->_material];

    shader.setBool("hasTexture", !this->_textures.empty());
    shader.setVec3("material.ambient", material.ambient);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);
    shader.setVec3("material.emission", material.emission);
    shader.setFloat("material.shininess", material.shininess);

    shader.setFloat("tex.shininess", 32.0f);
    glActiveTexture(GL_TEXTURE0 + this->_textures.size());
    shader.setInt("tex.diffuse", (int)this->_textures.size());
    shader.setInt("tex.emission", (int)this->_textures.size());
    shader.setInt("tex.specular", (int)this->_textures.size());
    glBindTexture(GL_TEXTURE_2D, 0);

    for (unsigned int i = 0; i < this->_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        shader.setInt(this->_textures[i].type, (int)i);
        glBindTexture(GL_TEXTURE_2D, textures[this->_textures[i].name]->getId());
    }

    glBindVertexArray(this->_bufferObject.vao);
    glDrawElements(GL_TRIANGLES, (int)this->_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    if (!model.textures.empty())
        glBindTexture(GL_TEXTURE_2D, 0);
}
