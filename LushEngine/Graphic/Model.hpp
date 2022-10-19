#ifndef MODEL_HPP
#define MODEL_HPP

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Graphic/Mesh.hpp"
#include "Includes.hpp"
#include "STB/stb_image.h"

namespace Lush
{
    typedef struct {
            int id;
            glm::mat4 offset;
    } BoneInfo;

    class Model
    {
        private:
            std::vector<Mesh> _meshes;
            std::map<std::string, BoneInfo> _boneInfoMap;
            int _boneCounter = 0;

        public:
            Model(std::string const &file, std::map<std::string, unsigned int> texturesLoaded);
            Model() = default;
            ~Model() = default;

            void load(std::string const &file, std::map<std::string, unsigned int> texturesLoaded);
            void draw(Shader &shader);

        private:
            std::map<std::string, BoneInfo> &getBoneInfoMap();
            int &getBoneCount();

            void processNode(aiNode &node, const aiScene &scene, std::map<std::string, unsigned int> texturesLoaded);
            Mesh processMesh(aiMesh &mesh, const aiScene &scene, std::map<std::string, unsigned int> texturesLoaded);
            std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::map<std::string, unsigned int> texturesLoaded);

            void setVertexBoneDataToDefault(Vertex &vertex);
            void setVertexBoneData(Vertex &vertex, int boneID, float weight);
            void extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh &mesh);
    };
}

#endif // MODEL_HPP
