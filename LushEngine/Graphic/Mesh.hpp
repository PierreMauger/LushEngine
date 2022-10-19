#ifndef MESH_HPP
#define MESH_HPP

#include "Graphic/Shader.hpp"
#include "Includes.hpp"

#define MAX_BONE_INFLUENCE 4

namespace Lush
{
    typedef struct {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoords;
            glm::vec3 tangent;
            glm::vec3 bitangent;
            int boneIDs[MAX_BONE_INFLUENCE];
            float weights[MAX_BONE_INFLUENCE];
    } Vertex;

    typedef struct {
            glm::vec3 diffuse;
            glm::vec3 specular;
            glm::vec3 ambient;
            glm::vec3 emission;
            float shininess;
    } Material;

    typedef struct {
            unsigned int id;
            std::string type;
    } Texture;

    class Mesh
    {
        private:
            std::vector<Vertex> _vertices;
            std::vector<unsigned int> _indices;
            std::vector<Texture> _textures;
            Material _material;
            unsigned int _VAO;
            unsigned int _VBO;
            unsigned int _EBO;

        public:
            Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material);
            ~Mesh() = default;
            void draw(Shader &shader);

        private:
            void setupMesh();
    };
}

#endif // MESH_HPP
