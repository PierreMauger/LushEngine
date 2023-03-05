#ifndef RENDERMODEL_HPP
#define RENDERMODEL_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Texture.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "STB/stb_image.h"

namespace Lush
{
    typedef struct {
            int id;
            glm::mat4 offset;
    } BoneInfo;

    class RenderModel : public Resource
    {
        private:
            std::vector<Mesh> _meshes;
            std::map<std::string, BoneInfo> _boneInfoMap;
            int _boneCounter = 0;

        public:
            RenderModel(File &file, std::unordered_map<std::string, Texture> textures);
            RenderModel() = default;
            ~RenderModel() = default;

            void load(File &file, std::unordered_map<std::string, Texture> textures);
            void reload(File &file, std::unordered_map<std::string, Texture> textures);
            void draw(Shader &shader);

        private:
            std::map<std::string, BoneInfo> &getBoneInfoMap();
            int &getBoneCount();

            void processNode(aiNode &node, const aiScene &scene, std::unordered_map<std::string, Texture> textures);
            Mesh processMesh(aiMesh &mesh, const aiScene &scene, std::unordered_map<std::string, Texture> textures);
            std::vector<Tex> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::unordered_map<std::string, Texture> textures);

            void setVertexBoneDataToDefault(Vertex &vertex);
            void setVertexBoneData(Vertex &vertex, int boneID, float weight);
            void extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh &mesh);
    };
}

#endif // RENDERMODEL_HPP
