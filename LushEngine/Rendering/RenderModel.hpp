#ifndef RENDER_MODEL_HPP
#define RENDER_MODEL_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Texture.hpp"
#include "STB/stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Lush
{
    struct BoneInfo {
            int id;
            glm::mat4 offset;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &id;
                ar &offset;
            }
    };

    class RenderModel : public Resource
    {
        private:
            std::vector<Mesh> _meshes;
            std::map<std::string, BoneInfo> _boneInfoMap;
            int _boneCounter = 0;

        public:
            RenderModel(File &file, std::unordered_map<std::string, Texture> textures);
            RenderModel() = default;
            ~RenderModel() override = default;

            void load(File &file, std::unordered_map<std::string, Texture> &textures);
            void reload(File &file, std::unordered_map<std::string, Texture> &textures);
            void draw(Shader &shader);

            std::vector<Mesh> &getMeshes();

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &_meshes;
                ar &_boneInfoMap;
                ar &_boneCounter;
            }

        private:
            // std::map<std::string, BoneInfo> &getBoneInfoMap();
            // int &getBoneCount();
            void processNode(aiNode &node, const aiScene &scene, std::unordered_map<std::string, Texture> &textures);
            Mesh processMesh(aiMesh &mesh, const aiScene &scene, std::unordered_map<std::string, Texture> &textures);
            static std::vector<Tex> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName, std::unordered_map<std::string, Texture> &textures);

            static void setVertexBoneDataToDefault(Vertex &vertex);
            static void setVertexBoneData(Vertex &vertex, int boneID, float weight);
            void extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh &mesh);
    };
}

#endif // RENDER_MODEL_HPP
