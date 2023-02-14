#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Includes.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/CubeMap.hpp"
#include "Rendering/RenderModel.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Map.hpp"
#include "Script/ScriptClass.hpp"
#include "Script/ScriptInstance.hpp"
#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Scene.hpp"

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

            std::shared_ptr<Scene> _scene;
            std::unique_ptr<MapMesh> _map;

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

            std::shared_ptr<Scene> getScene();
            MapMesh &getMap();
    };
}

#endif // RESOURCEMANAGER_HPP
