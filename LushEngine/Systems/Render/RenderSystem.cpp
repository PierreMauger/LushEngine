#include "Systems/Render/RenderSystem.hpp"

using namespace Lush;

std::string loadFile(std::string fileName);

RenderSystem::RenderSystem(std::shared_ptr<GLFWwindow> window) : _camera(1280, 720)
{
    this->_window = window;

    this->_models[0] = std::make_shared<Model>(loadFile("resources/models/Cube.dae"), std::map<std::string, unsigned int>());
    this->_shaders["Camera"] = std::make_shared<Shader>(loadFile("resources/shaders/camera.vs"), loadFile("resources/shaders/camera.fs"));

    this->_camera.setShaders(this->_shaders);
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
    auto &masks = entityManager.getMasks();
    std::size_t cam = (CompInfo::TRANSFORM | CompInfo::CAMERA);
    std::size_t renderable = (CompInfo::TRANSFORM | CompInfo::MODELID);
    std::size_t light = (CompInfo::TRANSFORM | CompInfo::LIGHT);

    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & light) == light) {
            Transform transform = componentManager.getComponent<Transform>(i);
            Light light = componentManager.getComponent<Light>(i);

            if (light.mod == 0)
                this->_dirLights.push_back({transform, light});
            else if (light.mod == 1)
                this->_pointLights.push_back({transform, light});
            else if (light.mod == 2)
                this->_spotLights.push_back({transform, light});
        }
    }

    this->_camera.use("Camera");
    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & cam) == cam) {
            Transform transform = componentManager.getComponent<Transform>(i);
            CameraComponent camera = componentManager.getComponent<CameraComponent>(i);

            this->_camera.update(transform, camera);
            this->_camera.setView(glfwGetTime());

            if (this->_dirLights.size() > 0)
                this->_camera.setDirLight(this->_dirLights[0]);
            this->_camera.setPointLights(this->_pointLights);
        }
    }
    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();

    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & renderable) == renderable) {
            Transform transform = componentManager.getComponent<Transform>(i);
            ModelID modelID = componentManager.getComponent<ModelID>(i);

            this->_camera.setOnModel(transform);
            if (this->_models.find(modelID.id) != this->_models.end())
                this->_models[modelID.id]->draw(*this->_camera.getShader());
        }
    }
}
