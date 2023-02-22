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
            std::map<std::string, File> _files;

            std::map<std::string, Shader> _shaders;
            std::map<std::string, Texture> _textures;
            std::map<std::string, RenderModel> _models;
            std::map<std::string, CubeMap> _skyboxes;
            std::map<std::string, ScriptClass> _scripts;
            std::vector<ScriptInstance> _instances;
            std::shared_ptr<ScriptPack> _nativeScriptPack;

            std::shared_ptr<Scene> _scene;
            std::unique_ptr<MapMesh> _map;
            MonoDomain *_domain;

            void initScriptDomain();
            void loadCoreScript();
            void loadDirectory(const std::filesystem::path &path, std::function<void(std::string)> func, const std::vector<std::string> &extensions);

        public:
            ResourceManager();
            ~ResourceManager();

            std::map<std::string, File> &getFiles();

            std::map<std::string, Shader> &getShaders();
            std::map<std::string, Texture> &getTextures();
            std::map<std::string, RenderModel> &getModels();
            std::map<std::string, CubeMap> &getSkyboxes();
            std::map<std::string, ScriptClass> &getScripts();
            std::vector<ScriptInstance> &getInstances();

            std::shared_ptr<ScriptPack> getScriptPack();
            std::shared_ptr<Scene> getScene();
            MapMesh &getMap();

            static ResourceManager *getResourceManager();

    };
}

#endif // RESOURCEMANAGER_HPP
