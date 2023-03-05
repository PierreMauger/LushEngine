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
    try {
        this->initScriptDomain();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    this->loadTextures("Resources/Textures");
    this->loadModels("Resources/Models");
    this->loadShaders("Resources/Shaders");
    this->loadSkyboxes("Resources/Skybox");
    this->loadScriptPacks("Resources/CoreScripts", "Core");
    this->loadScriptPacks("Resources/Scripts", "Native");
    this->loadScenes("Resources/Scenes");

    this->_map = std::make_unique<MapMesh>(256, 256);
}

ResourceManager::~ResourceManager()
{
    mono_jit_cleanup(this->_domain);
}

void ResourceManager::initScriptDomain()
{
    unsetenv("TERM");
    // setenv("MONO_PATH", "Resources/bin", 1);
    this->_domain = mono_jit_init("LushJIT");
    if (!this->_domain)
        throw std::runtime_error("mono_jit_init failed");
}

void ResourceManager::loadDirectory(const std::filesystem::path &path, std::function<void(std::string)> func, const std::vector<std::string> &extensions)
{
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            if (std::find(extensions.begin(), extensions.end(), entry.path().extension().string()) != extensions.end())
                func(entry.path().string());
        } else if (entry.is_directory()) {
            this->loadDirectory(entry.path(), func, extensions);
        }
    }
}

void ResourceManager::loadTextures(std::string dir)
{
    this->loadDirectory(dir, [this](std::string path) {
        this->_files[path] = File(path);
        this->_textures[std::filesystem::path(path).filename()] = Texture(this->_files[path]);
    }, {".png", ".jpg", ".jpeg"});
}

void ResourceManager::loadModels(std::string dir)
{
    this->loadDirectory(dir, [this](std::string path) {
        this->_files[path] = File(path);
        this->_models[this->_files[path].getName()] = RenderModel(this->_files[path], this->_textures);
    }, {".dae"});
}

void ResourceManager::loadShaders(std::string dir)
{
    this->loadDirectory(dir, [this](std::string path) {
        this->_files[path] = File(path);
    }, {".vs", ".fs", ".tcs", ".tes", ".gs", ".cs"});

    this->_shaders["Model"] = Shader(this->_files["Resources/Shaders/model.vs"], this->_files["Resources/Shaders/model.fs"]);
    this->_shaders["PickingModel"] = Shader(this->_files["Resources/Shaders/model.vs"], this->_files["Resources/Shaders/picking.fs"]);
    this->_shaders["PickingBillboard"] = Shader(this->_files["Resources/Shaders/billboard.vs"], this->_files["Resources/Shaders/picking.fs"]);
    this->_shaders["Outline"] = Shader(this->_files["Resources/Shaders/outline.vs"], this->_files["Resources/Shaders/outline.fs"]);
    this->_shaders["Skybox"] = Shader(this->_files["Resources/Shaders/skybox.vs"], this->_files["Resources/Shaders/skybox.fs"]);
    this->_shaders["Billboard"] = Shader(this->_files["Resources/Shaders/billboard.vs"], this->_files["Resources/Shaders/billboard.fs"]);
    this->_shaders["Grid"] = Shader(this->_files["Resources/Shaders/grid.vs"], this->_files["Resources/Shaders/grid.fs"]);
    this->_shaders["Map"] = Shader(this->_files["Resources/Shaders/map.vs"], this->_files["Resources/Shaders/map.fs"], File(), this->_files["Resources/Shaders/map.tcs"], this->_files["Resources/Shaders/map.tes"]);
    this->_shaders["CameraFrustum"] = Shader(this->_files["Resources/Shaders/cameraFrustum.vs"], this->_files["Resources/Shaders/cameraFrustum.fs"]);
}

void ResourceManager::loadSkyboxes(std::string dir)
{
    this->loadDirectory(dir, [this](std::string path) {
        this->_files[path] = File(path);
    }, {".jpg"});

    std::vector<File> files = {this->_files[dir + "/right.jpg"],  this->_files[dir + "/left.jpg"],  this->_files[dir + "/top.jpg"],
                               this->_files[dir + "/bottom.jpg"], this->_files[dir + "/front.jpg"], this->_files[dir + "/back.jpg"]};
    this->_skyboxes["Sky"] = CubeMap(files);
}

void ResourceManager::loadScriptPacks(std::string dir, std::string packName)
{
    this->loadDirectory(dir, [this](std::string path) {
        this->_files[path] = File(path);
    }, {".cs"});

    std::vector<File> tempFiles;
    for (auto &[name, file] : this->_files)
        if (name.find(dir) != std::string::npos)
            tempFiles.push_back(file);
    this->_scriptPacks[packName] = ScriptPack(tempFiles, packName, this->_scriptPacks);
    if (packName == "Core")
        return;
    for (auto &[name, klass] : this->_scriptPacks[packName].getClasses())
        this->_scripts[name] = ScriptClass(this->_scriptPacks[packName].getDomain(), klass, this->_scriptPacks[packName].getCoreClass());
}

void ResourceManager::loadScenes(std::string dir)
{
    this->loadDirectory(dir, [this](std::string path) {
        this->_files[path] = File(path);
        this->_scene = std::make_shared<Scene>(this->_files[path], this->_scripts);
    }, {".xml"});
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

std::unordered_map<std::string, CubeMap> &ResourceManager::getSkyboxes()
{
    return this->_skyboxes;
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

std::shared_ptr<Scene> ResourceManager::getScene()
{
    return this->_scene;
}

MapMesh &ResourceManager::getMap()
{
    return *this->_map;
}
