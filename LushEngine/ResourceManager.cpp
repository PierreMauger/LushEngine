#include "ResourceManager.hpp"

using namespace Lush;

static ResourceManager *resourceManager = nullptr;

ResourceManager *ResourceManager::getStaticResourceManager()
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
    this->reloadScripts(dir + "/Resources/Scripts");
    this->loadScenes(dir + "/Resources/Scenes");
}

void ResourceManager::loadEditor()
{
    this->loadTextures("Resources/Textures");
    this->loadModels("Resources/Models");
    this->loadShaders("Resources/Shaders");
    this->loadSkyBoxes("Resources/Skybox");
    this->loadScriptPack("Resources/ScriptsCore", "Core");
    this->loadScriptPack("Resources/ScriptsNative", "Game");
    this->loadScenes("Resources/Scenes");
}

void ResourceManager::loadGame()
{
    this->loadShaders("Resources/Shaders");
    this->loadSkyBoxes("Resources/Skybox");

    this->loadScriptDll("Data");
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
        oa << name;
        oa << model;
    }

    oa << this->_scenes.size();
    for (auto &[name, scene] : this->_scenes) {
        oa << name;
        oa << scene;
    }

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
        this->_textures[name].createTexture();
    }

    ia >> size;
    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        ia >> this->_models[name];
        for (auto &mesh : this->_models[name].getMeshes())
            mesh.rebindTextureIds(this->_textures);
    }

    ia >> size;
    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ia >> name;
        ia >> this->_scenes[name];
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

void ResourceManager::initScriptInstances(EntityManager &entityManager)
{
    for (auto &[id, entity] : entityManager.getEntities()) {
        entity.clearScriptIndexes();
        for (auto &[name, script] : this->_scripts) {
            if (entity.hasScriptComponent(name)) {
                entity.addScriptIndex(name, this->_instances.size());
                this->_instances.emplace_back(script, id, entity.getScriptComponent(name).getFields());
            }
        }
    }
    for (auto &instance : this->_instances)
        instance.init();
}

void ResourceManager::initPhysicInstances(EntityManager &entityManager)
{
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (entity.hasComponent<Transform>()) {
            if (entity.hasComponent<CharacterController>()) {
                if (entity.hasComponent<Collider>())
                    this->_characterInstances.emplace_back(id, entity.getComponent<Transform>(), entity.getComponent<CharacterController>(), entity.getComponent<Collider>());
                else
                    this->_characterInstances.emplace_back(id, entity.getComponent<Transform>(), entity.getComponent<CharacterController>());

            } else if (entity.hasComponent<RigidBody>()) {
                if (entity.hasComponent<Collider>())
                    this->_physicInstances.emplace_back(id, entity.getComponent<Transform>(), entity.getComponent<RigidBody>(), entity.getComponent<Collider>());
                else
                    this->_physicInstances.emplace_back(id, entity.getComponent<Transform>(), entity.getComponent<RigidBody>());

            } else if (entity.hasComponent<Map>()) {
                // TODO terrain instance ? useful if communication with scripts, maybe fluid heightmap ?
                Texture &texture = this->_textures[entity.getComponent<Map>().heightMap];
                Transform &transform = entity.getComponent<Transform>();

                btHeightfieldTerrainShape *terrainShape =
                    new btHeightfieldTerrainShape(texture.getWidth(), texture.getHeight(), texture.getData(), 32.0f / 256.0f, 0.0f, 32.0f, 1, PHY_UCHAR, false);
                terrainShape->setUseDiamondSubdivision(true); // Enable diamond subdivision for better terrain smoothness
                terrainShape->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
                btDefaultMotionState *motionState = new btDefaultMotionState();
                btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, terrainShape);
                btRigidBody *terrainBody = new btRigidBody(rbInfo);

                btTransform btTransform;
                btTransform.setIdentity();
                btTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
                glm::quat rotation = glm::quat(glm::radians(transform.rotation));
                btTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
                terrainBody->setWorldTransform(btTransform);

                this->_dynamicsWorld->addRigidBody(terrainBody);
            }
        }
    }

    for (auto &physicInstance : this->_physicInstances)
        this->_dynamicsWorld->addRigidBody(physicInstance.getRigidBody());

    for (auto &characterInstance : this->_characterInstances) {
        this->_dynamicsWorld->addCollisionObject(characterInstance.getGhostObject(), btBroadphaseProxy::CharacterFilter,
                                                 btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
        this->_dynamicsWorld->addAction(characterInstance.getCharacterController());
    }
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
    this->_gamePack = std::make_shared<ScriptPack>(files, "Game");

    for (auto &[name, klass] : this->_gamePack->getClasses())
        this->_scripts[name] = ScriptClass(this->_gamePack->getDomain(), klass, this->_corePack->getClasses()["Component"]);
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
        this->_gamePack = std::make_shared<ScriptPack>(tempFiles, packName);
        for (auto &[name, klass] : this->_gamePack->getClasses())
            this->_scripts[name] = ScriptClass(this->_gamePack->getDomain(), klass, this->_corePack->getClasses()["Component"]);
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
    for (auto &file : this->_gamePack->getFiles())
        tempFiles.push_back(file);
    this->_gamePack->reload(tempFiles);
    for (auto &[name, klass] : this->_gamePack->getClasses())
        this->_scripts[name] = ScriptClass(this->_gamePack->getDomain(), klass, this->_corePack->getClasses()["Component"]);
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
    return this->_corePack->getClasses()["Component"];
}

std::shared_ptr<ScriptPack> &ResourceManager::getGamePack()
{
    return this->_gamePack;
}

std::unordered_map<std::string, ScriptClass> &ResourceManager::getScripts()
{
    return this->_scripts;
}

std::vector<ScriptInstance> &ResourceManager::getScriptInstances()
{
    return this->_instances;
}

std::vector<PhysicInstance> &ResourceManager::getPhysicInstances()
{
    return this->_physicInstances;
}

std::vector<CharacterInstance> &ResourceManager::getCharacterInstances()
{
    return this->_characterInstances;
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

void ResourceManager::setDynamicsWorld(btDiscreteDynamicsWorld *world)
{
    this->_dynamicsWorld = world;
}

void ResourceManager::resetDynamicsWorld()
{
    int numBodies = this->_dynamicsWorld->getNumCollisionObjects();
    for (int i = numBodies - 1; i >= 0; --i) {
        btCollisionObject *obj = this->_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody *body = btRigidBody::upcast(obj);
        if (body) {
            this->_dynamicsWorld->removeRigidBody(body);

            delete body->getMotionState();
            delete body->getCollisionShape();
            delete body;
        }
    }
}
