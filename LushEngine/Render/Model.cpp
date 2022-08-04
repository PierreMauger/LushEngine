#include "Model.hpp"

using namespace Lush;

// the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from)
{
    glm::mat4 to;

    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

Model::Model(std::string const &filename)
{
    this->load(filename);
}

std::map<std::string, BoneInfo> &Model::getBoneInfoMap()
{
    return this->_boneInfoMap;
}

int &Model::getBoneCount()
{
    return this->_boneCounter;
}

void Model::load(std::string const &filename)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                           aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene)
        throw std::runtime_error(importer.GetErrorString());
    this->processNode(*scene->mRootNode, *scene);
}

void Model::processNode(aiNode &node, const aiScene &scene)
{
    for (unsigned int i = 0; i < node.mNumMeshes; i++) {
        aiMesh *mesh = scene.mMeshes[node.mMeshes[i]];
        this->_meshes.push_back(this->processMesh(*mesh, scene));
    }
    for (unsigned int i = 0; i < node.mNumChildren; i++)
        this->processNode(*node.mChildren[i], scene);
}

void Model::setVertexBoneDataToDefault(Vertex &vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

void Model::setVertexBoneData(Vertex &vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.boneIDs[i] < 0) {
            vertex.weights[i] = weight;
            vertex.boneIDs[i] = boneID;
            break;
        }
    }
}

void Model::extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh &mesh)
{
    auto &boneInfoMap = this->_boneInfoMap;
    int &boneCount = this->_boneCounter;

    for (unsigned int boneIndex = 0; boneIndex < mesh.mNumBones; ++boneIndex) {
        int boneID = -1;
        std::string boneName = mesh.mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh.mBones[boneIndex]->mOffsetMatrix);
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

Mesh Model::processMesh(aiMesh &mesh, const aiScene &scene)
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
    materialLoaded->Get(AI_MATKEY_SHININESS, shininess);
    material.shininess = shininess;

    this->extractBoneWeightForVertices(vertices, mesh);

    std::vector<Texture> diffuseMaps =
        this->loadMaterialTextures(materialLoaded, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps =
        this->loadMaterialTextures(materialLoaded, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Texture> normalMaps =
        this->loadMaterialTextures(materialLoaded, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<Texture> heightMaps =
        this->loadMaterialTextures(materialLoaded, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    if (textures.size()) {
    }

    return Mesh(vertices, indices, textures, material);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < this->_texturesLoaded.size(); j++) {
            if (std::strcmp(this->_texturesLoaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(this->_texturesLoaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = textureFromFile(str.C_Str(), "resources/textures/");
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            this->_texturesLoaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int Model::textureFromFile(const char *path, const std::string &directory, [[maybe_unused]] bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    // int width, height, nrComponents;

    // Image image = LoadImage(filename.c_str());
    // Texture2D texture = LoadTextureFromImage(image);
    // return texture.id;
    return 0;
}

void Model::draw(Shader &shader)
{
    for (unsigned int i = 0; i < this->_meshes.size(); i++)
        this->_meshes[i].draw(shader);
}
