#include "ResourceManager.hpp"

using namespace Lush;

static ResourceManager *resourceManager = nullptr;

ResourceManager *ResourceManager::getResourceManager()
{
    return resourceManager;
}

ResourceManager::ResourceManager()
{
    resourceManager = this;
    this->_mapMesh = std::make_unique<MapMesh>();
}

ResourceManager::~ResourceManager()
{
    mono_jit_cleanup(this->_domain);
}

void ResourceManager::loadProject(const std::string &dir)
{
    this->loadTextures(dir + "/Resources/Textures");
    this->loadModels(dir + "/Resources/Models");
    this->loadScenes(dir + "/Resources/Scenes");
    this->reloadScripts(dir + "/Resources/Scripts");
}

void ResourceManager::loadEditor()
{
    this->loadTextures("Resources/Textures");
    this->loadModels("Resources/Models");
    this->loadShaders("Resources/Shaders");
    this->loadSkyBoxes("Resources/Skybox");
    this->loadScriptPack("Resources/CoreScripts", "Core");
    this->loadScriptPack("Resources/Scripts", "Native");
    this->loadScenes("Resources/Scenes");
}

void ResourceManager::loadGame()
{
    this->loadShaders("Resources/Shaders");
    this->loadSkyBoxes("Resources/Skybox");
    this->loadScriptDll("Data");
    this->loadScenes("Resources/Scenes");

    this->deserializeAssetPack("Data/AssetPack.data");
}

void ResourceManager::serializeAssetPack(std::string path)
{
    std::ofstream ofs(path, std::ios::binary);
    boost::archive::binary_oarchive oa(ofs, boost::archive::no_header);

    oa << this->_textures.size();
    for (auto &[name, texture] : this->_textures) {
        oa << name;
        oa << texture;
    }

    oa << this->_models.size();
    for (auto &[name, model] : this->_models) {
        // if (model.isUsed(name, this->_scenes)) {
            oa << name;
            oa << model;
        // }
    }

    //    oa << this->_shaders.size();
    //    for (auto &[name, shader] : this->_shaders) {
    //        oa << name;
    //        oa << shader;
    //    }

    //    oa << this->_scenes.size();
    //    for (auto &[name, scene] : this->_scenes) {
    //        oa << name;
    //        oa << scene;
    //    }

    ofs.close();
}

void ResourceManager::deserializeAssetPack(std::string path)
{
    std::ifstream ifs(path, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs, boost::archive::no_header);
    std::size_t size;

    ia >> size;
    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        ia >> this->_textures[name];
    }

    ia >> size;
    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        ia >> this->_models[name];
        for (auto &mesh : this->_models[name].getMeshes())
            mesh.rebindTexIds(this->_textures);
    }

    ifs.close();
}

void ResourceManager::initScriptDomain(const std::string &dir)
{
    unsetenv("TERM");
    mono_set_dirs(dir.c_str(), "libs");
    // setenv("MONO_LOG_LEVEL", "debug", 1);
    // setenv("MONO_LOG_MASK", "dll,cfg", 1);

    this->_domain = mono_jit_init("LushJIT");
    if (!this->_domain)
        throw std::runtime_error("mono_jit_init failed");
    mono_thread_set_main(mono_thread_current());
}

void ResourceManager::loadDirectory(const std::filesystem::path &path, const std::function<void(const std::string &)> &func, const std::vector<std::string> &extensions)
{
    if (!std::filesystem::exists(path))
        return;
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file() && std::find(extensions.begin(), extensions.end(), entry.path().extension().string()) != extensions.end())
            func(entry.path().string());
        else if (entry.is_directory())
            this->loadDirectory(entry.path(), func, extensions);
    }
}

void ResourceManager::loadTextures(const std::string &dir)
{
    this->loadDirectory(dir,
                        [this](const std::string &path) {
                            this->_files[path] = File(path);
                            this->_textures[std::filesystem::path(path).filename()] = Texture(this->_files[path]);
                        },
                        {".png", ".jpg", ".jpeg"});
}

void ResourceManager::loadModels(const std::string &dir)
{
    this->loadDirectory(dir,
                        [this](const std::string &path) {
                            this->_files[path] = File(path);
                            this->_models[this->_files[path].getName()] = RenderModel(this->_files[path], this->_textures);
                        },
                        {".dae"});
}

void ResourceManager::loadShaders(const std::string &dir)
{
    this->loadDirectory(dir,
                        [this](const std::string &path) {
                            this->_files[path] = File(path);
                            this->_shaders[this->_files[path].getName()] = Shader(this->_files[path]);
                        },
                        {".glsl"});
}

void ResourceManager::loadSkyBoxes(const std::string &dir)
{
    this->loadDirectory(dir, [this](const std::string &path) { this->_files[path] = File(path); }, {".jpg"});

    std::vector<File> files = {this->_files[dir + "/right.jpg"],  this->_files[dir + "/left.jpg"],  this->_files[dir + "/top.jpg"],
                               this->_files[dir + "/bottom.jpg"], this->_files[dir + "/front.jpg"], this->_files[dir + "/back.jpg"]};
    this->_skyBoxes["Sky"] = CubeMap(files);
}

void ResourceManager::loadScriptDll(const std::string &dir)
{
    std::vector<File> coreFiles = {File(dir + "/Core.dll")};
    std::vector<File> files = {File(dir + "/Game.dll")};

    this->_corePack = std::make_unique<ScriptPack>(coreFiles, "Core");
    this->_scriptPacks["Game"] = ScriptPack(files, "Game");

    for (auto &[name, klass] : this->_scriptPacks["Game"].getClasses())
        this->_scripts[name] = ScriptClass(this->_scriptPacks["Game"].getDomain(), klass, this->_corePack->getClasses()["Entity"]);
}

void ResourceManager::loadScriptPack(const std::string &dir, const std::string &packName)
{
    static std::vector<File> tempFiles;
    this->loadDirectory(dir,
                        [this](const std::string &path) {
                            this->_files[path] = File(path);
                            tempFiles.push_back(this->_files[path]);
                        },
                        {".cs"});
    if (packName != "Core") {
        this->_scriptPacks["Game"] = ScriptPack(tempFiles, "Game");
        for (auto &[name, klass] : this->_scriptPacks["Game"].getClasses())
            this->_scripts[name] = ScriptClass(this->_scriptPacks["Game"].getDomain(), klass, this->_corePack->getClasses()["Entity"]);
    } else {
        this->_corePack = std::make_unique<ScriptPack>(tempFiles, packName);
    }
    tempFiles.clear();
}

void ResourceManager::reloadScripts(const std::string &dir)
{
    static std::vector<File> tempFiles;
    this->loadDirectory(dir,
                        [this](const std::string &path) {
                            this->_files[path] = File(path);
                            tempFiles.push_back(this->_files[path]);
                        },
                        {".cs"});
    for (auto &file : this->_scriptPacks["Game"].getFiles())
        tempFiles.push_back(file);
    this->_scriptPacks["Game"].reload(tempFiles);
    for (auto &[name, klass] : this->_scriptPacks["Game"].getClasses())
        this->_scripts[name] = ScriptClass(this->_scriptPacks["Game"].getDomain(), klass, this->_corePack->getClasses()["Entity"]);
    tempFiles.clear();
}

void ResourceManager::loadScenes(const std::string &dir)
{
    this->loadDirectory(dir,
                        [this](const std::string &path) {
                            this->_files[path] = File(path);
                            this->_scenes[this->_files[path].getName()] = Scene(this->_files[path], this->_scripts);
                        },
                        {".xml"});
}

std::unordered_map<std::string, File> &ResourceManager::getFiles()
{
    return this->_files;
}

std::unordered_map<std::string, Shader> &ResourceManager::getShaders()
{
    return this->_shaders;
}

std::unordered_map<std::string, Texture> &ResourceManager::getTextures()
{
    return this->_textures;
}

std::unordered_map<std::string, RenderModel> &ResourceManager::getModels()
{
    return this->_models;
}

std::unordered_map<std::string, CubeMap> &ResourceManager::getSkyBoxes()
{
    return this->_skyBoxes;
}

MonoClass *ResourceManager::getEntityClass()
{
    return this->_corePack->getClasses()["Entity"];
}

std::unordered_map<std::string, ScriptPack> &ResourceManager::getScriptPacks()
{
    return this->_scriptPacks;
}

std::unordered_map<std::string, ScriptClass> &ResourceManager::getScripts()
{
    return this->_scripts;
}

std::vector<ScriptInstance> &ResourceManager::getInstances()
{
    return this->_instances;
}

std::unordered_map<std::string, Scene> &ResourceManager::getScenes()
{
    return this->_scenes;
}

std::string ResourceManager::getActiveScene() const
{
    return this->_activeScene;
}

void ResourceManager::setActiveScene(const std::string &name)
{
    this->_activeScene = name;
}

MapMesh &ResourceManager::getMapMesh()
{
    return *this->_mapMesh;
}
