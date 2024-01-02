#include "ResourceManager.hpp"

using namespace Lush;

static ResourceManager *resourceManager = nullptr;

ResourceManager *ResourceManager::getStaticResourceManager()
{
    return resourceManager;
}

ResourceManager::ResourceManager(const std::string &resourceDir)
{
    resourceManager = this;
    this->_mapMesh = std::make_unique<MapMesh>();
    this->initScriptDomain(resourceDir);
}

ResourceManager::~ResourceManager()
{
    if (this->_domain)
        mono_jit_cleanup(this->_domain);
}

void ResourceManager::loadProject(const std::string &dir)
{
    this->loadTextures(dir + "/Resources/Textures");
    this->loadModels(dir + "/Resources/Models");
    this->loadSkyboxes(dir + "/Resources/Skyboxes");
    this->reloadScripts(dir + "/Resources/Scripts");
    this->loadScenes(dir + "/Resources/Scenes");
}

void ResourceManager::loadEditor()
{
    this->loadTextures("Resources/Textures");
    this->loadModels("Resources/Models");
    this->loadShaders("Resources/Shaders");
    this->loadSkyboxes("Resources/Skyboxes");
    this->loadScriptPack("Resources/ScriptsCore", "Core");
    this->loadScriptPack("Resources/ScriptsNative", "Game");
    this->loadScenes("Resources/Scenes");
}

void ResourceManager::loadGame()
{
    this->loadScriptDll("Data");
    this->deserializeAssetPack("Data/AssetPack.data");
}

void ResourceManager::setAllResourcesUsage()
{
    for (auto &[name, model] : this->_models)
        model->setUsed(false);
    for (auto &[name, skybox] : this->_skyboxes)
        skybox->setUsed(false);
    for (auto &[name, texture] : this->_textures)
        texture->setUsed(false);

    for (auto &[modelName, model] : this->_models)
        if (this->isModelUsed(modelName)) {
            model->setUsed(true);
            for (auto textureName : model->getTextureNames())
                this->_textures[textureName]->setUsed(true);
        }
    for (auto &[skyboxName, skybox] : this->_skyboxes)
        if (this->isSkyboxUsed(skyboxName))
            skybox->setUsed(true);
    for (auto &[textureName, texture] : this->_textures)
        if (this->isTextureUsed(textureName))
            texture->setUsed(true);
    this->_textures[this->_logoName]->setUsed(true);
}

bool ResourceManager::isModelUsed(std::string modelName)
{
    for (auto &[name, scene] : this->_scenes) {
        if (!scene->isUsed())
            continue;
        for (auto &[id, entity] : scene->getEntityManager()->getEntities())
            if (entity.hasComponent<Model>() && entity.getComponent<Model>().name == modelName)
                return true;
    }
    return false;
}

bool ResourceManager::isSkyboxUsed(std::string skyboxName)
{
    for (auto &[name, scene] : this->_scenes) {
        if (!scene->isUsed())
            continue;
        for (auto &[id, entity] : scene->getEntityManager()->getEntities())
            if (entity.hasComponent<Cubemap>() && entity.getComponent<Cubemap>().name == skyboxName)
                return true;
    }
    return false;
}

bool ResourceManager::isTextureUsed(std::string textureName)
{
    for (auto &[name, scene] : this->_scenes) {
        if (!scene->isUsed())
            continue;
        for (auto &[id, entity] : scene->getEntityManager()->getEntities()) {
            if (entity.hasComponent<Billboard>() && entity.getComponent<Billboard>().name == textureName)
                return true;
            if (entity.hasComponent<Map>() && (entity.getComponent<Map>().heightMap == textureName || entity.getComponent<Map>().diffuseTexture == textureName ||
                                               entity.getComponent<Map>().normalTexture == textureName || entity.getComponent<Map>().diffuseTexture2 == textureName ||
                                               entity.getComponent<Map>().diffuseTexture3 == textureName))
                return true;
        }
    }
    return false;
}

void ResourceManager::serializeAssetPack(std::string path)
{
    std::ofstream ofs(path, std::ios::binary);
    boost::archive::binary_oarchive oa(ofs, boost::archive::no_header);

    this->setAllResourcesUsage();

    oa << std::ranges::count_if(this->_textures, [](const auto &texture) { return texture.second->isUsed(); });
    for (auto &[name, texture] : this->_textures) {
        if (!texture->isUsed())
            continue;
        oa << name;
        oa << *texture;
    }

    oa << std::ranges::count_if(this->_models, [](const auto &model) { return model.second->isUsed(); });
    for (auto &[name, model] : this->_models) {
        if (!model->isUsed())
            continue;
        oa << name;
        oa << *model;
    }

    oa << std::ranges::count_if(this->_skyboxes, [](const auto &skybox) { return skybox.second->isUsed(); });
    for (auto &[name, skybox] : this->_skyboxes) {
        if (!skybox->isUsed())
            continue;
        oa << name;
        oa << *skybox;
    }

    oa << this->_shaders.size();
    for (auto &[name, shader] : this->_shaders) {
        oa << name;
        oa << *shader;
    }

    oa << this->_scenes.size();
    for (auto &[name, scene] : this->_scenes) {
        oa << name;
        oa << *scene;
    }

    oa << this->_logoName;

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
        this->_textures[name] = std::make_unique<Texture>();
        ia >> *this->_textures[name];
        this->_textures[name]->createTexture();
    }

    ia >> size;
    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        this->_models[name] = std::make_unique<RenderModel>();
        ia >> *this->_models[name];
        for (auto &mesh : this->_models[name]->getMeshes())
            mesh.rebindTextureIds(this->_textures);
    }

    ia >> size;
    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        this->_skyboxes[name] = std::make_unique<Skybox>();
        ia >> *this->_skyboxes[name];
        this->_skyboxes[name]->createSkybox();
    }

    ia >> size;
    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        this->_shaders[name] = std::make_unique<Shader>();
        ia >> *this->_shaders[name];
        this->_shaders[name]->createShader();
    }

    ia >> size;
    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        this->_scenes[name] = std::make_unique<Scene>();
        ia >> *this->_scenes[name];
    }

    ia >> this->_logoName;

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

void ResourceManager::initScriptInstances(std::shared_ptr<EntityManager> &entityManager)
{
    for (auto &[id, entity] : entityManager->getEntities()) {
        entity.clearScriptIndexes();
        for (auto &[name, script] : this->_scripts) {
            if (entity.hasScriptComponent(name)) {
                entity.addScriptIndex(name, this->_scriptInstances.size());
                this->_scriptInstances.emplace_back(script, id, entity.getScriptComponent(name).getFields());
            }
        }
    }
    for (auto &instance : this->_scriptInstances)
        instance.init();
}

void ResourceManager::initPhysicInstances(std::shared_ptr<EntityManager> &entityManager)
{
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (entity.hasComponent<Transform>()) {
            if (entity.hasComponent<CharacterController>()) {
                if (entity.hasComponent<Collider>())
                    this->_physicInstances.emplace_back(
                        std::make_unique<CharacterInstance>(id, entity.getComponent<Transform>(), entity.getComponent<CharacterController>(), entity.getComponent<Collider>()));
                else
                    this->_physicInstances.emplace_back(std::make_unique<CharacterInstance>(id, entity.getComponent<Transform>(), entity.getComponent<CharacterController>()));

            } else if (entity.hasComponent<RigidBody>()) {
                if (entity.hasComponent<Collider>())
                    this->_physicInstances.emplace_back(
                        std::make_unique<PhysicInstance>(id, entity.getComponent<Transform>(), entity.getComponent<RigidBody>(), entity.getComponent<Collider>()));
                else
                    this->_physicInstances.emplace_back(std::make_unique<PhysicInstance>(id, entity.getComponent<Transform>(), entity.getComponent<RigidBody>()));
            } else if (entity.hasComponent<Map>()) {
                if (entity.hasComponent<Collider>())
                    this->_physicInstances.emplace_back(
                        std::make_unique<TerrainInstance>(id, entity.getComponent<Transform>(), *this->_textures[entity.getComponent<Map>().heightMap]));
            } else if (entity.hasComponent<Collider>()) {
                this->_physicInstances.emplace_back(std::make_unique<PhysicInstance>(id, entity.getComponent<Transform>(), entity.getComponent<Collider>()));
            }
        }
    }

    for (auto &physicInstance : this->_physicInstances)
        physicInstance->addToWorld(this->_dynamicsWorld);
}

void ResourceManager::loadDirectoryFiles(const std::filesystem::path &path, const std::function<void(const std::string &)> &func, const std::vector<std::string> &extensions)
{
    if (!std::filesystem::exists(path))
        return;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        std::string fullPath = entry.path().string();
        if (entry.is_regular_file() && std::find(extensions.begin(), extensions.end(), entry.path().extension().string()) != extensions.end()) {
            this->_files[fullPath] = File(fullPath);
            func(fullPath);
        }
    }
}

void ResourceManager::loadDirectoryNewFiles(const std::filesystem::path &path, const std::function<void(const std::string &)> &func, const std::vector<std::string> &extensions)
{
    if (!std::filesystem::exists(path))
        return;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        std::string fullPath = entry.path().string();
        if (entry.is_regular_file() && !this->_files.contains(fullPath) && std::find(extensions.begin(), extensions.end(), entry.path().extension().string()) != extensions.end()) {
            this->_files[fullPath] = File(fullPath);
            func(fullPath);
        }
    }
}

void ResourceManager::loadShaders(const std::string &dir)
{
    this->loadDirectoryFiles(dir, [this](const std::string &path) { this->_shaders[this->_files[path].getName()] = std::make_unique<Shader>(this->_files[path]); }, {".glsl"});
}

void ResourceManager::loadTextures(const std::string &dir)
{
    this->loadDirectoryFiles(dir, [this](const std::string &path) { this->_textures[std::filesystem::path(path).filename()] = std::make_unique<Texture>(this->_files[path]); },
                             {".png", ".jpg", ".jpeg"});
}

void ResourceManager::loadModels(const std::string &dir)
{
    this->loadDirectoryFiles(
        dir, [this](const std::string &path) { this->_models[this->_files[path].getName()] = std::make_unique<RenderModel>(this->_files[path], this->_textures); }, {".dae"});
}

void ResourceManager::loadSkyboxes(const std::string &dir)
{
    this->loadDirectoryFiles(dir, [this](const std::string &path) { this->_skyboxes[this->_files[path].getName()] = std::make_unique<Skybox>(this->_files[path]); },
                             {".png", ".jpg", ".jpeg"});
}

void ResourceManager::loadScenes(const std::string &dir)
{
    this->loadDirectoryFiles(dir, [this](const std::string &path) { this->_scenes[this->_files[path].getName()] = std::make_unique<Scene>(this->_files[path], this->_scripts); },
                             {".xml"});
}

void ResourceManager::loadScriptDll(const std::string &dir)
{
    std::vector<File> coreFiles = {File(dir + "/Core.dll")};
    std::vector<File> files = {File(dir + "/Game.dll")};

    this->_corePack = std::make_unique<ScriptPack>(coreFiles, "Core");
    this->_gamePack = std::make_unique<ScriptPack>(files, "Game");

    for (auto &[name, klass] : this->_gamePack->getClasses())
        this->_scripts[name] = ScriptClass(this->_gamePack->getDomain(), klass, this->getComponentClass());
}

void ResourceManager::loadScriptPack(const std::string &dir, const std::string &packName)
{
    std::vector<File> tempFiles;
    this->loadDirectoryFiles(dir, [&tempFiles, this](const std::string &path) { tempFiles.push_back(this->_files[path]); }, {".cs"});
    if (packName != "Core") {
        this->_gamePack = std::make_unique<ScriptPack>(tempFiles, packName);
        for (auto &[name, klass] : this->_gamePack->getClasses())
            this->_scripts[name] = ScriptClass(this->_gamePack->getDomain(), klass, this->getComponentClass());
    } else {
        this->_corePack = std::make_unique<ScriptPack>(tempFiles, packName);
    }
}

void ResourceManager::reloadScripts(const std::string &dir)
{
    std::vector<File> tempFiles;
    this->loadDirectoryFiles(dir, [&tempFiles, this](const std::string &path) { tempFiles.push_back(this->_files[path]); }, {".cs"});
    for (auto &file : this->_gamePack->getFiles())
        tempFiles.push_back(file);
    this->_gamePack->reload(tempFiles);
    for (auto &[name, klass] : this->_gamePack->getClasses())
        this->_scripts[name] = ScriptClass(this->_gamePack->getDomain(), klass, this->getComponentClass());
}

std::unordered_map<std::string, File> &ResourceManager::getFiles()
{
    return this->_files;
}

std::unordered_map<std::string, std::unique_ptr<Shader>> &ResourceManager::getShaders()
{
    return this->_shaders;
}

std::unordered_map<std::string, std::unique_ptr<Texture>> &ResourceManager::getTextures()
{
    return this->_textures;
}

std::unordered_map<std::string, std::unique_ptr<RenderModel>> &ResourceManager::getModels()
{
    return this->_models;
}

std::unordered_map<std::string, std::unique_ptr<Skybox>> &ResourceManager::getSkyboxes()
{
    return this->_skyboxes;
}

MonoClass *ResourceManager::getComponentClass()
{
    return this->_corePack->getClasses()["Component"];
}

std::unique_ptr<ScriptPack> &ResourceManager::getGamePack()
{
    return this->_gamePack;
}

std::unordered_map<std::string, ScriptClass> &ResourceManager::getScripts()
{
    return this->_scripts;
}

std::vector<ScriptInstance> &ResourceManager::getScriptInstances()
{
    return this->_scriptInstances;
}

std::vector<std::unique_ptr<BasicInstance>> &ResourceManager::getPhysicInstances()
{
    return this->_physicInstances;
}

std::unordered_map<std::string, std::unique_ptr<Scene>> &ResourceManager::getScenes()
{
    return this->_scenes;
}

Scene &ResourceManager::getActiveScene()
{
    return *this->_scenes[this->_activeScene];
}

std::string ResourceManager::getActiveSceneName() const
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

Texture &ResourceManager::getLogo()
{
    return *this->_textures[this->_logoName];
}

void ResourceManager::setLogo(const std::string &name)
{
    this->_logoName = name;
}

btDiscreteDynamicsWorld *ResourceManager::getDynamicsWorld() const
{
    return this->_dynamicsWorld;
}

void ResourceManager::setDynamicsWorld(btDiscreteDynamicsWorld *world)
{
    this->_dynamicsWorld = world;
}

void ResourceManager::resetDynamicsWorld()
{
    for (auto &instance : this->_physicInstances)
        instance->removeFromWorld(this->_dynamicsWorld);
}

void ResourceManager::setSceneChanged(bool sceneChanged)
{
    this->_sceneChanged = sceneChanged;
}

bool ResourceManager::isSceneChanged() const
{
    return this->_sceneChanged;
}
