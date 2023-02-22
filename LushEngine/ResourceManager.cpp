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

    this->loadDirectory("Resources/Textures", [this](std::string path) {
        this->_files[path] = File(path);
        std::string name = std::filesystem::path(path).filename().string();
        this->_textures[name] = Texture(this->_files[path]);
    }, {".png", ".jpg", ".jpeg"});

    this->loadDirectory("Resources/Models", [this](std::string path) {
        this->_files[path] = File(path);
        this->_models[this->_files[path].getName()] = RenderModel(this->_files[path], this->_textures);
    }, {".dae"});

    try {
        this->initScriptDomain();
        this->loadCoreScript();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    this->loadDirectory("Resources/Scripts", [this](std::string path) { this->_files[path] = File(path); }, {".cs"});
    std::vector<File> tempFiles;
    for (auto &[name, file] : this->_files)
        if (name.find("Resources/Scripts") != std::string::npos)
            tempFiles.push_back(file);
    this->_nativeScriptPack = std::make_unique<ScriptPack>(tempFiles, "Native");
    for (auto &[name, klass] : this->_nativeScriptPack->getClasses())
        this->_scripts[name] = ScriptClass(this->_nativeScriptPack->getDomain(), klass, this->_nativeScriptPack->getCoreClass());

    this->loadDirectory("Resources/Scenes", [this](std::string path) {
        this->_files[path] = File(path);
        this->_scene = std::make_shared<Scene>(this->_files[path], this->_scripts);
    }, {".xml"});

    this->loadDirectory("Resources/Shaders", [this](std::string path) { this->_files[path] = File(path); }, {".vs", ".fs", ".tcs", ".tes", ".gs", ".cs"});
    this->_shaders["Model"] = Shader(this->_files["Resources/Shaders/model.vs"], this->_files["Resources/Shaders/model.fs"]);
    this->_shaders["PickingModel"] = Shader(this->_files["Resources/Shaders/model.vs"], this->_files["Resources/Shaders/picking.fs"]);
    this->_shaders["PickingBillboard"] = Shader(this->_files["Resources/Shaders/billboard.vs"], this->_files["Resources/Shaders/picking.fs"]);
    this->_shaders["Outline"] = Shader(this->_files["Resources/Shaders/outline.vs"], this->_files["Resources/Shaders/outline.fs"]);
    this->_shaders["Skybox"] = Shader(this->_files["Resources/Shaders/skybox.vs"], this->_files["Resources/Shaders/skybox.fs"]);
    this->_shaders["Billboard"] = Shader(this->_files["Resources/Shaders/billboard.vs"], this->_files["Resources/Shaders/billboard.fs"]);
    this->_shaders["Grid"] = Shader(this->_files["Resources/Shaders/grid.vs"], this->_files["Resources/Shaders/grid.fs"]);
    this->_shaders["Map"] = Shader(this->_files["Resources/Shaders/map.vs"], this->_files["Resources/Shaders/map.fs"], File(), this->_files["Resources/Shaders/map.tcs"], this->_files["Resources/Shaders/map.tes"]);

    this->loadDirectory("Resources/Skybox", [this](std::string path) { this->_files[path] = File(path); }, {".jpg"});
    std::vector<File> files = {this->_files["Resources/Skybox/right.jpg"],  this->_files["Resources/Skybox/left.jpg"],  this->_files["Resources/Skybox/top.jpg"],
                               this->_files["Resources/Skybox/bottom.jpg"], this->_files["Resources/Skybox/front.jpg"], this->_files["Resources/Skybox/back.jpg"]};
    this->_skyboxes["Sky"] = CubeMap(files);

    this->_map = std::make_unique<MapMesh>(2624, 1756);
}

ResourceManager::~ResourceManager()
{
    mono_jit_cleanup(this->_domain);
}

void ResourceManager::initScriptDomain()
{
    unsetenv("TERM");
    setenv("MONO_PATH", "Resources/CoreScripts", 1);
    this->_domain = mono_jit_init("LushJIT");
    if (!this->_domain)
        throw std::runtime_error("mono_jit_init failed");
}

void ResourceManager::loadCoreScript()
{
    if (system("mcs -target:library -out:Resources/CoreScripts/Core.dll Resources/CoreScripts/Components.cs Resources/CoreScripts/InternalCalls.cs Resources/CoreScripts/Entity.cs"))
        throw std::runtime_error("mcs failed");
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

std::map<std::string, Shader> &ResourceManager::getShaders()
{
    return this->_shaders;
}

std::map<std::string, Texture> &ResourceManager::getTextures()
{
    return this->_textures;
}

std::map<std::string, RenderModel> &ResourceManager::getModels()
{
    return this->_models;
}

std::map<std::string, CubeMap> &ResourceManager::getSkyboxes()
{
    return this->_skyboxes;
}

std::map<std::string, File> &ResourceManager::getFiles()
{
    return this->_files;
}

std::map<std::string, ScriptClass> &ResourceManager::getScripts()
{
    return this->_scripts;
}

std::vector<ScriptInstance> &ResourceManager::getInstances()
{
    return this->_instances;
}

std::shared_ptr<ScriptPack> ResourceManager::getScriptPack()
{
    return this->_nativeScriptPack;
}

std::shared_ptr<Scene> ResourceManager::getScene()
{
    return this->_scene;
}

MapMesh &ResourceManager::getMap()
{
    return *this->_map;
}
