#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <btBulletDynamicsCommon.h>

#include "ECS/ECS.hpp"
#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "Physic/CharacterInstance.hpp"
#include "Physic/PhysicInstance.hpp"
#include "Physic/TerrainInstance.hpp"
#include "Rendering/MapMesh.hpp"
#include "Rendering/RenderModel.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Skybox.hpp"
#include "Rendering/Texture.hpp"
#include "Scene.hpp"
#include "Script/ScriptClass.hpp"
#include "Script/ScriptInstance.hpp"
#include "Script/ScriptPack.hpp"
#include "Serialization.hpp"

namespace Lush
{
    class ResourceManager
    {
        private:
            std::unordered_map<std::string, File> _files;
            std::vector<std::string> _directories;

            std::unordered_map<std::string, std::shared_ptr<Shader>> _shaders;
            std::unordered_map<std::string, std::unique_ptr<Texture>> _textures;
            std::unordered_map<std::string, std::unique_ptr<RenderModel>> _models;
            std::unordered_map<std::string, std::unique_ptr<Skybox>> _skyboxes;
            std::unordered_map<std::string, std::unique_ptr<Scene>> _scenes;
            std::unique_ptr<ScriptPack> _corePack;
            std::unique_ptr<ScriptPack> _gamePack;

            std::unordered_map<std::string, ScriptClass> _scripts;
            std::map<std::size_t, ScriptInstance> _scriptInstances;
            std::vector<std::unique_ptr<BasicInstance>> _physicInstances;

            std::string _logoName = "Lush.png";
            std::string _activeScene;
            bool _sceneChanged = false;

            std::unique_ptr<MapMesh> _mapMesh;
            MonoDomain *_domain = nullptr;
            btDiscreteDynamicsWorld *_dynamicsWorld = nullptr;

            void initScriptDomain(const std::string &dir);
            void loadDirectoryFiles(const std::filesystem::path &path, const std::function<void(const std::string &)> &func, const std::vector<std::string> &extensions);

            void loadShaders(const std::string &dir);
            void loadTextures(const std::string &dir);
            void loadModels(const std::string &dir);
            void loadSkyboxes(const std::string &dir);
            void loadScenes(const std::string &dir);
            void loadScriptDll(const std::string &dir);
            void loadScriptPack(const std::string &dir, const std::string &name);
            void reloadScripts(const std::string &dir);

            void setAllResourcesUsage();

            bool isModelUsed(std::string modelName);
            bool isSkyboxUsed(std::string skyboxName);
            bool isTextureUsed(std::string textureName);

        public:
            ResourceManager(const std::string &resourceDir);
            ~ResourceManager();

            void loadProject(const std::string &dir);
            void loadEditor();
            void loadGame();

            void loadDirectoryNewFiles(const std::filesystem::path &path, const std::function<void(const std::string &)> &func, const std::vector<std::string> &extensions);

            void initScriptInstances(std::shared_ptr<EntityManager> &entityManager);
            void initPhysicInstances(std::shared_ptr<EntityManager> &entityManager);

            void serializeAssetPack(std::string path);
            void deserializeAssetPack(std::string path);

            std::unordered_map<std::string, File> &getFiles();

            std::unordered_map<std::string, std::shared_ptr<Shader>> &getShaders();
            std::unordered_map<std::string, std::unique_ptr<Texture>> &getTextures();
            std::unordered_map<std::string, std::unique_ptr<RenderModel>> &getModels();
            std::unordered_map<std::string, std::unique_ptr<Skybox>> &getSkyboxes();
            std::unordered_map<std::string, std::unique_ptr<Scene>> &getScenes();
            std::unique_ptr<ScriptPack> &getGamePack();

            MonoClass *getComponentClass();
            std::unordered_map<std::string, ScriptClass> &getScripts();
            std::map<std::size_t, ScriptInstance> &getScriptInstances();
            std::vector<std::unique_ptr<BasicInstance>> &getPhysicInstances();

            Scene &getActiveScene();
            std::string getActiveSceneName() const;
            void setActiveScene(const std::string &name);
            Texture &getLogo();
            void setLogo(const std::string &name);

            MapMesh &getMapMesh();
            btDiscreteDynamicsWorld *getDynamicsWorld() const;
            void setDynamicsWorld(btDiscreteDynamicsWorld *dynamicsWorld);
            void resetDynamicsWorld();

            void setSceneChanged(bool sceneChanged);
            bool isSceneChanged() const;

            static ResourceManager *getStaticResourceManager();
    };
}

#endif // RESOURCE_MANAGER_HPP
