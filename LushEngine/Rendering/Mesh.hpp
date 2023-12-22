#ifndef MESH_HPP
#define MESH_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "Includes.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Serialization.hpp"

#define MAX_BONE_INFLUENCE 4

namespace Lush
{
    struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoords;
            glm::vec3 tangent;
            glm::vec3 bitangent;
            int boneIDs[MAX_BONE_INFLUENCE];
            float weights[MAX_BONE_INFLUENCE];

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &position;
                ar &normal;
                ar &texCoords;
                ar &tangent;
                ar &bitangent;
                // ar &boneIDs;
                // ar &weights;
            }
    };

    struct Material {
            glm::vec3 diffuse;
            glm::vec3 specular;
            glm::vec3 ambient;
            glm::vec3 emission;
            float shininess;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &diffuse;
                ar &specular;
                ar &ambient;
                ar &emission;
                ar &shininess;
            }
    };

    struct Tex {
            unsigned int id;
            std::string name;
            std::string type;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &name;
                ar &type;
            }
    };

    struct BufferObject {
            GLuint vao;
            GLuint vbo;
            GLuint ebo;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &vao;
                ar &vbo;
                ar &ebo;
            }
    };

    struct FrameBuffer {
            GLuint framebuffer;
            GLuint texture;
            GLuint depthbuffer;
    };

    class Mesh
    {
        private:
            std::vector<Vertex> _vertices;
            std::vector<unsigned int> _indices;
            std::vector<Tex> _textures;
            Material _material{};
            BufferObject _bufferObject{};

            void setupMesh();

        public:
            Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Tex> &textures, Material material);
            Mesh() = default;
            ~Mesh() = default;

            void rebindTextureIds(std::unordered_map<std::string, std::unique_ptr<Texture>> &textures);
            void draw(Shader &shader);
            std::vector<std::string> getTextureNames();

            void serialize(boost::archive::binary_iarchive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &_vertices;
                ar &_indices;
                ar &_textures;
                ar &_material;
                ar &_bufferObject;
                this->setupMesh();
            }

            void serialize(boost::archive::binary_oarchive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &_vertices;
                ar &_indices;
                ar &_textures;
                ar &_material;
                ar &_bufferObject;
            }
    };
}

#endif // MESH_HPP
