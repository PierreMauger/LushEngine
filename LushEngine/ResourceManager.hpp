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
#include "Physic/PhysicInstance.hpp"
#include "Physic/CharacterInstance.hpp"
#include "Rendering/CubeMap.hpp"
#include "Rendering/MapMesh.hpp"
#include "Rendering/RenderModel.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/Shader.hpp"
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

            std::unordered_map<std::string, Shader> _shaders;
            std::unordered_map<std::string, Texture> _textures;
            std::unordered_map<std::string, RenderModel> _models;
            std::unordered_map<std::string, CubeMap> _skyBoxes;
            std::unique_ptr<ScriptPack> _corePack;
            std::shared_ptr<ScriptPack> _gamePack;
            std::unordered_map<std::string, ScriptClass> _scripts;
            std::vector<ScriptInstance> _scriptInstances;
            std::vector<std::unique_ptr<BasicInstance>> _physicInstances;
            std::unordered_map<std::string, Scene> _scenes;
            std::string _activeScene;

            std::unique_ptr<MapMesh> _mapMesh;
            MonoDomain *_domain = nullptr;
            btDiscreteDynamicsWorld *_dynamicsWorld = nullptr;

            void loadDirectory(const std::filesystem::path &path, const std::function<void(const std::string &)> &func, const std::vector<std::string> &extensions);

            void loadTextures(const std::string &dir);
            void loadModels(const std::string &dir);
            void loadShaders(const std::string &dir);
            void loadSkyBoxes(const std::string &dir);
            void loadScriptDll(const std::string &dir);
            void loadScriptPack(const std::string &dir, const std::string &name);
            void reloadScripts(const std::string &dir);
            void loadScenes(const std::string &dir);

            // void setUsage();

        public:
            ResourceManager();
            ~ResourceManager();

            void loadProject(const std::string &dir);
            void loadEditor();
            void loadGame();
            void initScriptDomain(const std::string &dir);

            void initScriptInstances(EntityManager &entityManager);
            void initPhysicInstances(EntityManager &entityManager);

            void serializeAssetPack(std::string path);
            void deserializeAssetPack(std::string path);

            std::unordered_map<std::string, File> &getFiles();

            std::unordered_map<std::string, Shader> &getShaders();
            std::unordered_map<std::string, Texture> &getTextures();
            std::unordered_map<std::string, RenderModel> &getModels();
            std::unordered_map<std::string, CubeMap> &getSkyBoxes();
            MonoClass *getEntityClass();
            std::shared_ptr<ScriptPack> &getGamePack();
            std::unordered_map<std::string, ScriptClass> &getScripts();
            std::vector<ScriptInstance> &getScriptInstances();
            std::vector<std::unique_ptr<BasicInstance>> &getPhysicInstances();

            std::unordered_map<std::string, Scene> &getScenes();
            std::string getActiveScene() const;
            void setActiveScene(const std::string &name);
            MapMesh &getMapMesh();

            btDiscreteDynamicsWorld *getDynamicsWorld() const;
            void setDynamicsWorld(btDiscreteDynamicsWorld *dynamicsWorld);
            void resetDynamicsWorld();

            static ResourceManager *getStaticResourceManager();
    };
}

#endif // RESOURCE_MANAGER_HPP
