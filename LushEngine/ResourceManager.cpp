#include "ResourceManager.hpp"

using namespace Lush;

ResourceManager::ResourceManager()
{
    this->_files["Fox"] = File("Resources/Models/Fox.dae");
    this->_files["Crate"] = File("Resources/Models/Crate.dae");
    this->_files["Cube"] = File("Resources/Models/Cube.dae");
    this->_files["Maxwell"] = File("Resources/Models/Maxwell.dae");

    this->_files["model.vs"] = File("Resources/Shaders/model.vs");
    this->_files["model.fs"] = File("Resources/Shaders/model.fs");
    this->_files["picking.fs"] = File("Resources/Shaders/picking.fs");
    this->_files["billboard.vs"] = File("Resources/Shaders/billboard.vs");
    this->_files["billboard.fs"] = File("Resources/Shaders/billboard.fs");
    this->_files["outline.vs"] = File("Resources/Shaders/outline.vs");
    this->_files["outline.fs"] = File("Resources/Shaders/outline.fs");
    this->_files["skybox.vs"] = File("Resources/Shaders/skybox.vs");
    this->_files["skybox.fs"] = File("Resources/Shaders/skybox.fs");
    this->_files["grid.vs"] = File("Resources/Shaders/grid.vs");
    this->_files["grid.fs"] = File("Resources/Shaders/grid.fs");
    this->_files["map.vs"] = File("Resources/Shaders/map.vs");
    this->_files["map.fs"] = File("Resources/Shaders/map.fs");
    this->_files["map.tcs"] = File("Resources/Shaders/map.tcs");
    this->_files["map.tes"] = File("Resources/Shaders/map.tes");

    this->_files["Spin"] = File("Resources/Scripts/Spin.cs");
    this->_files["MaxwellScript"] = File("Resources/Scripts/Maxwell.cs");
    this->_files["Controlable"] = File("Resources/Scripts/Controlable.cs");

    this->_files["Crate.png"] = File("Resources/Textures/Crate.png");
    this->_files["Crate_specular.png"] = File("Resources/Textures/Crate_specular.png");
    this->_files["Crate_emission.png"] = File("Resources/Textures/Crate_emission.png");
    this->_files["Maxwell.jpeg"] = File("Resources/Textures/Maxwell.jpeg");
    this->_files["Whiskers.png"] = File("Resources/Textures/Whiskers.png");
    this->_files["heightMap.png"] = File("Resources/Textures/heightMap.png");
    this->_files["Audio.png"] = File("Resources/Textures/Editor/Audio.png");
    this->_files["Camera.png"] = File("Resources/Textures/Editor/Camera.png");
    this->_files["DirectionalLight.png"] = File("Resources/Textures/Editor/DirectionalLight.png");
    this->_files["Geometry.png"] = File("Resources/Textures/Editor/Geometry.png");
    this->_files["PointLight.png"] = File("Resources/Textures/Editor/PointLight.png");
    this->_files["SpotLight.png"] = File("Resources/Textures/Editor/SpotLight.png");

    this->_files["scene.xml"] = File("Resources/Scenes/scene.xml");

    this->_scene = std::make_shared<Scene>(this->_files["scene.xml"]);

    this->_textures["Crate.png"] = Texture(this->_files["Crate.png"]);
    this->_textures["Crate_specular.png"] = Texture(this->_files["Crate_specular.png"]);
    this->_textures["Crate_emission.png"] = Texture(this->_files["Crate_emission.png"]);
    this->_textures["Maxwell.jpeg"] = Texture(this->_files["Maxwell.jpeg"]);
    this->_textures["Whiskers.png"] = Texture(this->_files["Whiskers.png"]);
    this->_textures["heightMap.png"] = Texture(this->_files["heightMap.png"]);
    this->_textures["Audio.png"] = Texture(this->_files["Audio.png"]);
    this->_textures["Camera.png"] = Texture(this->_files["Camera.png"]);
    this->_textures["DirectionalLight.png"] = Texture(this->_files["DirectionalLight.png"]);
    this->_textures["Geometry.png"] = Texture(this->_files["Geometry.png"]);
    this->_textures["PointLight.png"] = Texture(this->_files["PointLight.png"]);
    this->_textures["SpotLight.png"] = Texture(this->_files["SpotLight.png"]);

    this->_models["Fox"] = RenderModel(this->_files["Fox"], this->_textures);
    this->_models["Crate"] = RenderModel(this->_files["Crate"], this->_textures);
    this->_models["Cube"] = RenderModel(this->_files["Cube"], this->_textures);
    this->_models["Maxwell"] = RenderModel(this->_files["Maxwell"], this->_textures);

    this->_files["right.jpg"] = File("Resources/Skybox/right.jpg");
    this->_files["left.jpg"] = File("Resources/Skybox/left.jpg");
    this->_files["top.jpg"] = File("Resources/Skybox/top.jpg");
    this->_files["bottom.jpg"] = File("Resources/Skybox/bottom.jpg");
    this->_files["front.jpg"] = File("Resources/Skybox/front.jpg");
    this->_files["back.jpg"] = File("Resources/Skybox/back.jpg");

    std::vector<File> files = {this->_files["right.jpg"], this->_files["left.jpg"], this->_files["top.jpg"], this->_files["bottom.jpg"],
                               this->_files["front.jpg"], this->_files["back.jpg"]};
    this->_skyboxes["Sky"] = CubeMap(files);

    this->_shaders["Model"] = Shader(this->_files["model.vs"], this->_files["model.fs"]);
    this->_shaders["PickingModel"] = Shader(this->_files["model.vs"], this->_files["picking.fs"]);
    this->_shaders["PickingBillboard"] = Shader(this->_files["billboard.vs"], this->_files["picking.fs"]);
    this->_shaders["Outline"] = Shader(this->_files["outline.vs"], this->_files["outline.fs"]);
    this->_shaders["Skybox"] = Shader(this->_files["skybox.vs"], this->_files["skybox.fs"]);
    this->_shaders["Billboard"] = Shader(this->_files["billboard.vs"], this->_files["billboard.fs"]);
    this->_shaders["Grid"] = Shader(this->_files["grid.vs"], this->_files["grid.fs"]);
    this->_shaders["Map"] = Shader(this->_files["map.vs"], this->_files["map.fs"], File(), this->_files["map.tcs"], this->_files["map.tes"]);

    this->_map = std::make_unique<MapMesh>(2624, 1756);
}

ResourceManager::~ResourceManager()
{
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

MapMesh &ResourceManager::getMap()
{
    return *this->_map;
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

std::shared_ptr<Scene> ResourceManager::getScene()
{
    return this->_scene;
}
