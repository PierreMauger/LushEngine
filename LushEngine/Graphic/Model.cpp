#include "Model.hpp"

using namespace Lush;

// the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from)
{
    glm::mat4 to;

    to[0][0] = from.a1;
    to[1][0] = from.a2;
    to[2][0] = from.a3;
    to[3][0] = from.a4;
    to[0][1] = from.b1;
    to[1][1] = from.b2;
    to[2][1] = from.b3;
    to[3][1] = from.b4;
    to[0][2] = from.c1;
    to[1][2] = from.c2;
    to[2][2] = from.c3;
    to[3][2] = from.c4;
    to[0][3] = from.d1;
    to[1][3] = from.d2;
    to[2][3] = from.d3;
    to[3][3] = from.d4;
    return to;
}

RenderModel::RenderModel(std::string const &file, std::map<std::string, unsigned int> texturesLoaded)
{
    this->load(file, texturesLoaded);
}

std::map<std::string, BoneInfo> &RenderModel::getBoneInfoMap()
{
    return this->_boneInfoMap;
}

int &RenderModel::getBoneCount()
{
    return this->_boneCounter;
}

void RenderModel::load(std::string const &file, std::map<std::string, unsigned int> texturesLoaded)
{
    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFileFromMemory(file.c_str(), file.size(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene)
        throw std::runtime_error(std::string("RenderModel loading: ") + importer.GetErrorString());
    this->processNode(*scene->mRootNode, *scene, texturesLoaded);
}

void RenderModel::processNode(aiNode &node, const aiScene &scene, std::map<std::string, unsigned int> texturesLoaded)
{
    for (unsigned int i = 0; i < node.mNumMeshes; i++)
        this->_meshes.push_back(this->processMesh(*scene.mMeshes[node.mMeshes[i]], scene, texturesLoaded));
    for (unsigned int i = 0; i < node.mNumChildren; i++)
        this->processNode(*node.mChildren[i], scene, texturesLoaded);
}

void RenderModel::setVertexBoneDataToDefault(Vertex &vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void RenderModel::setVertexBoneData(Vertex &vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.boneIDs[i] < 0) {
            vertex.weights[i] = weight;
            vertex.boneIDs[i] = boneID;
            break;
        }
    }
}

void RenderModel::extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh &mesh)
{
    auto &boneInfoMap = this->_boneInfoMap;
    int &boneCount = this->_boneCounter;

    for (unsigned int boneIndex = 0; boneIndex < mesh.mNumBones; ++boneIndex) {
        int boneID = -1;
        std::string boneName = mesh.mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            BoneInfo newBoneInfo = {boneCount, ConvertMatrixToGLMFormat(mesh.mBones[boneIndex]->mOffsetMatrix)};
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
        } else {
            boneID = boneInfoMap[boneName].id;
        }
        if (boneID == -1)
            continue;
        auto weights = mesh.mBones[boneIndex]->mWeights;
        int numWeights = mesh.mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
            unsigned int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            if (vertexId <= vertices.size())
                this->setVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

Mesh RenderModel::processMesh(aiMesh &mesh, const aiScene &scene, std::map<std::string, unsigned int> texturesLoaded)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
        Vertex vertex;
        this->setVertexBoneDataToDefault(vertex);
        vertex.position = glm::vec3(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z);
        vertex.normal = glm::vec3(mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z);

        if (mesh.mTextureCoords[0])
            vertex.texCoords = glm::vec2(mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y);
        else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
        aiFace face = mesh.mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial *materialLoaded = scene.mMaterials[mesh.mMaterialIndex];
    aiColor3D color(0.0f, 0.0f, 0.0f);
    float shininess = 0.0f;

    Material material;

    materialLoaded->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    material.diffuse = glm::vec3(color.r, color.g, color.b);
    materialLoaded->Get(AI_MATKEY_COLOR_AMBIENT, color);
    material.ambient = glm::vec3(color.r, color.g, color.b);
    materialLoaded->Get(AI_MATKEY_COLOR_SPECULAR, color);
    material.specular = glm::vec3(color.r, color.g, color.b);
    materialLoaded->Get(AI_MATKEY_COLOR_EMISSIVE, color);
    material.emission = glm::vec3(color.r, color.g, color.b);
    materialLoaded->Get(AI_MATKEY_SHININESS, shininess);
    material.shininess = shininess;

    this->extractBoneWeightForVertices(vertices, mesh);

    std::vector<Texture> diffuseMaps = this->loadMaterialTextures(materialLoaded, aiTextureType_DIFFUSE, "tex.diffuse", texturesLoaded);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = this->loadMaterialTextures(materialLoaded, aiTextureType_SPECULAR, "tex.specular", texturesLoaded);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Texture> emissiveMaps = this->loadMaterialTextures(materialLoaded, aiTextureType_EMISSIVE, "tex.emission", texturesLoaded);
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    std::vector<Texture> normalMaps = this->loadMaterialTextures(materialLoaded, aiTextureType_NORMALS, "tex.normal", texturesLoaded);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<Texture> heightMaps = this->loadMaterialTextures(materialLoaded, aiTextureType_HEIGHT, "tex.height", texturesLoaded);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Mesh(vertices, indices, textures, material);
}

std::vector<Texture> RenderModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::map<std::string, unsigned int> texturesLoaded)
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        if (texturesLoaded.find(str.C_Str()) == texturesLoaded.end())
            throw std::runtime_error("Couldn't find texture: " + std::string(str.C_Str()));
        textures.push_back({texturesLoaded[str.C_Str()], typeName});
    }
    return textures;
}

void RenderModel::draw(Shader &shader)
{
    for (unsigned int i = 0; i < this->_meshes.size(); i++)
        this->_meshes[i].draw(shader);
}
