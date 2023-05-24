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
    // this->loadScriptPacks(dir + "/Resources/Scripts", std::filesystem::path(dir).filename());
    this->loadScenes(dir + "/Resources/Scenes");
}

void ResourceManager::buildAssetPack()
{
    std::ofstream ofs("Resources/AssetPack.data", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs, boost::archive::no_header);

    oa << this->_models.size();
    for (auto &[name, model] : this->_models) {
        oa << name;
        oa << model;
    }

    //    oa << this->_textures.size();
    //    for (auto &[name, texture] : this->_textures) {
    //        oa << name;
    //        oa << texture;
    //    }

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

    ofs.flush();
    ofs.close();
}

void ResourceManager::deserialize()
{
    std::ifstream ifs("Resources/AssetPack.data", std::ios::binary);
    boost::archive::binary_iarchive ia(ifs, boost::archive::no_header);

    size_t size;
    ia >> size;
    for (size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        ia >> this->_models[name];
    }
    ifs.close();
}

void ResourceManager::initScriptDomain()
{
    unsetenv("TERM");
    // setenv("MONO_PATH", "Resources/bin", 1);

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

void ResourceManager::loadScriptsDll(const std::string &dir)
{
    std::vector<File> coreFiles = {File(dir + "/Core.dll")};
    std::vector<File> files = {File(dir + "/Game.dll")};

    this->_scriptPacks["Core"] = ScriptPack(coreFiles, "Core", nullptr);
    this->_scriptPacks["Game"] = ScriptPack(files, "Game", this->_scriptPacks["Core"].getClasses()["Entity"]);

    for (auto &[name, klass] : this->_scriptPacks["Game"].getClasses()) {
        this->_scripts[name] = ScriptClass(this->_scriptPacks["Game"].getDomain(), klass, this->_scriptPacks["Game"].getEntityClass());
        ECS::getECS()->getComponentManager().bindInstanceFields(name);
        ECS::getECS()->getEntityManager().addMaskCategory(ComponentType::COMPONENT_TYPE_COUNT << this->getScripts().size());
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
    this->loadDirectory(dir, [this](const std::string &path) { this->_files[path] = File(path); }, {".vs", ".fs", ".tcs", ".tes", ".gs", ".cs"});

    this->_shaders["Model"] = Shader(this->_files["Resources/Shaders/model.vs"], this->_files["Resources/Shaders/model.fs"]);
    this->_shaders["PickingModel"] = Shader(this->_files["Resources/Shaders/model.vs"], this->_files["Resources/Shaders/picking.fs"]);
    this->_shaders["PickingBillboard"] = Shader(this->_files["Resources/Shaders/billboard.vs"], this->_files["Resources/Shaders/picking.fs"]);
    this->_shaders["Outline"] = Shader(this->_files["Resources/Shaders/outline.vs"], this->_files["Resources/Shaders/outline.fs"]);
    this->_shaders["Skybox"] = Shader(this->_files["Resources/Shaders/skybox.vs"], this->_files["Resources/Shaders/skybox.fs"]);
    this->_shaders["Billboard"] = Shader(this->_files["Resources/Shaders/billboard.vs"], this->_files["Resources/Shaders/billboard.fs"]);
    this->_shaders["Grid"] = Shader(this->_files["Resources/Shaders/grid.vs"], this->_files["Resources/Shaders/grid.fs"]);
    this->_shaders["Map"] = Shader(this->_files["Resources/Shaders/map.vs"], this->_files["Resources/Shaders/map.fs"], File(), this->_files["Resources/Shaders/map.tcs"],
                                   this->_files["Resources/Shaders/map.tes"]);
    this->_shaders["MapWireframe"] =
        Shader(this->_files["Resources/Shaders/map.vs"], this->_files["Resources/Shaders/mapWireframe.fs"], this->_files["Resources/Shaders/mapWireframe.gs"],
               this->_files["Resources/Shaders/map.tcs"], this->_files["Resources/Shaders/map.tes"]);
    this->_shaders["CameraFrustum"] = Shader(this->_files["Resources/Shaders/cameraFrustum.vs"], this->_files["Resources/Shaders/cameraFrustum.fs"]);
    this->_shaders["Game"] = Shader(this->_files["Resources/Shaders/outline.vs"], this->_files["Resources/Shaders/billboard.fs"]);
}

void ResourceManager::loadSkyBoxes(const std::string &dir)
{
    this->loadDirectory(dir, [this](const std::string &path) { this->_files[path] = File(path); }, {".jpg"});

    std::vector<File> files = {this->_files[dir + "/right.jpg"],  this->_files[dir + "/left.jpg"],  this->_files[dir + "/top.jpg"],
                               this->_files[dir + "/bottom.jpg"], this->_files[dir + "/front.jpg"], this->_files[dir + "/back.jpg"]};
    this->_skyBoxes["Sky"] = CubeMap(files);
}

void ResourceManager::loadScriptPacks(const std::string &dir, const std::string &packName)
{
    static std::vector<File> tempFiles;
    this->loadDirectory(dir,
                        [this](const std::string &path) {
                            this->_files[path] = File(path);
                            tempFiles.push_back(this->_files[path]);
                        },
                        {".cs"});
    if (packName != "Core") {
        this->_scriptPacks["Game"] = ScriptPack(tempFiles, "Game", this->_scriptPacks["Core"].getClasses()["Entity"]);
        for (auto &[name, klass] : this->_scriptPacks["Game"].getClasses()) {
            this->_scripts[name] = ScriptClass(this->_scriptPacks["Game"].getDomain(), klass, this->_scriptPacks["Game"].getEntityClass());
            ECS::getECS()->getComponentManager().bindInstanceFields(name);
            ECS::getECS()->getEntityManager().addMaskCategory(ComponentType::COMPONENT_TYPE_COUNT << this->getScripts().size());
        }
    } else {
        this->_scriptPacks[packName] = ScriptPack(tempFiles, packName, nullptr);
    }
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

void ResourceManager::loadEditor()
{
    this->loadTextures("Resources/Textures");
    this->loadModels("Resources/Models");
    this->loadShaders("Resources/Shaders");
    this->loadSkyBoxes("Resources/Skybox");
    this->loadScriptPacks("Resources/CoreScripts", "Core");
    this->loadScriptPacks("Resources/Scripts", "Native");
    this->loadScenes("Resources/Scenes");
}

void ResourceManager::loadGame()
{
    this->loadTextures("Resources/Textures");
    this->loadShaders("Resources/Shaders");
    this->loadSkyBoxes("Resources/Skybox");
    this->loadScriptsDll("Resources/bin");
    this->loadScenes("Resources/Scenes");

    this->deserialize();
}
