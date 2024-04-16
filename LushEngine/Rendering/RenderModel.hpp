#ifndef RENDER_MODEL_HPP
#define RENDER_MODEL_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Texture.hpp"
#include "Scene.hpp"
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
                ar & id;
                ar & offset;
            }
    };

    class RenderModel : public Resource
    {
        private:
            std::vector<Mesh> _meshes;
            std::unordered_map<std::string, Material> _defaultMaterials;
            std::vector<Tex> _defaultTextures;
            std::unordered_map<std::string, BoneInfo> _boneInfoMap;
            int _boneCounter = 0;

            // std::unordered_map<std::string, BoneInfo> &getBoneInfoMap();
            // int &getBoneCount();
            void processNode(aiNode &node, const aiScene &scene);
            Mesh processMesh(aiMesh &mesh, const aiScene &scene);
            static std::vector<Tex> getTexturesFromMaterial(aiMaterial *mat, aiTextureType type, const std::string &typeName);

            static void setVertexBoneDataToDefault(Vertex &vertex);
            static void setVertexBoneData(Vertex &vertex, int boneID, float weight);
            void extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh &mesh);

        public:
            explicit RenderModel(File &file);
            RenderModel() = default;
            ~RenderModel() = default;

            void load(const File &file);
            void reload(const File &file);
            void draw(Shader &shader, Model &model, std::unordered_map<std::string, std::unique_ptr<Texture>> &textures);

            int getMaterialNb() const;
            int getTextureNb() const;
            std::unordered_map<std::string, Material> &getMaterials();
            std::vector<std::string> getTextureNames();

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar & _meshes;
                ar & _boneInfoMap;
                ar & _boneCounter;
            }
    };
}

#endif // RENDER_MODEL_HPP
