#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "Rendering/CubeMap.hpp"
#include "Rendering/Map.hpp"
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
            std::unordered_map<std::string, CubeMap> _skyboxes;
            std::unordered_map<std::string, ScriptPack> _scriptPacks;
            std::unordered_map<std::string, ScriptClass> _scripts;
            std::vector<ScriptInstance> _instances;

            std::shared_ptr<Scene> _scene;
            std::unique_ptr<MapMesh> _map;
            MonoDomain *_domain;

            void initScriptDomain();
            void loadDirectory(const std::filesystem::path &path, std::function<void(std::string)> func, const std::vector<std::string> &extensions);

            void loadTextures(std::string dir);
            void loadModels(std::string dir);
            void loadShaders(std::string dir);
            void loadSkyboxes(std::string dir);
            void loadScriptPacks(std::string dir, std::string name);
            void loadScenes(std::string dir);

        public:
            ResourceManager();
            ~ResourceManager();

            std::unordered_map<std::string, File> &getFiles();

            std::unordered_map<std::string, Shader> &getShaders();
            std::unordered_map<std::string, Texture> &getTextures();
            std::unordered_map<std::string, RenderModel> &getModels();
            std::unordered_map<std::string, CubeMap> &getSkyboxes();
            std::unordered_map<std::string, ScriptPack> &getScriptPacks();
            std::unordered_map<std::string, ScriptClass> &getScripts();
            std::vector<ScriptInstance> &getInstances();

            std::shared_ptr<Scene> getScene();
            MapMesh &getMap();

            static ResourceManager *getResourceManager();

    };
}

#endif // RESOURCEMANAGER_HPP
