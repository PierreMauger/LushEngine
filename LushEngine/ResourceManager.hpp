#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ECS.hpp"
#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
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
            std::unordered_map<std::string, ScriptPack> _scriptPacks;
            std::unordered_map<std::string, ScriptClass> _scripts;
            std::vector<ScriptInstance> _instances;
            std::unordered_map<std::string, Scene> _scenes;
            std::string _activeScene;

            std::unique_ptr<MapMesh> _mapMesh;
            MonoDomain *_domain = nullptr;

            void initScriptDomain();
            void loadDirectory(const std::filesystem::path &path, const std::function<void(const std::string &)> &func, const std::vector<std::string> &extensions);
            void deserialize();

            void loadTextures(const std::string &dir);
            void loadModels(const std::string &dir);
            void loadShaders(const std::string &dir);
            void loadSkyBoxes(const std::string &dir);
            void loadScriptPacks(const std::string &dir, const std::string &name);
            void loadScenes(const std::string &dir);

        public:
            ResourceManager();
            ~ResourceManager();

            void loadProject(const std::string &dir);
            void loadEditor();
            void loadGame();
            void build();

            std::unordered_map<std::string, File> &getFiles();

            std::unordered_map<std::string, Shader> &getShaders();
            std::unordered_map<std::string, Texture> &getTextures();
            std::unordered_map<std::string, RenderModel> &getModels();
            std::unordered_map<std::string, CubeMap> &getSkyBoxes();
            std::unordered_map<std::string, ScriptPack> &getScriptPacks();
            std::unordered_map<std::string, ScriptClass> &getScripts();
            std::vector<ScriptInstance> &getInstances();

            std::unordered_map<std::string, Scene> &getScenes();
            std::string getActiveScene() const;
            void setActiveScene(const std::string &name);
            MapMesh &getMapMesh();

            static ResourceManager *getResourceManager();
    };
}

#endif // RESOURCE_MANAGER_HPP
