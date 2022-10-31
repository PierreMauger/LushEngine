#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;
}

void CameraSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    std::size_t light = (ComponentType::TRANSFORM | ComponentType::LIGHT);
    std::size_t cam = (ComponentType::TRANSFORM | ComponentType::CAMERA);

    for (auto &id : entityManager.getMaskCategory(light)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Light light = componentManager.getComponent<Light>(id);

        if (light.type == LightType::DIRECTIONAL)
            this->_dirLights.push_back({transform, light});
        else if (light.type == LightType::POINT)
            this->_pointLights.push_back({transform, light});
        else if (light.type == LightType::SPOT)
            this->_spotLights.push_back({transform, light});
    }

    this->_graphic->getCamera().use("Camera");
    for (auto &id : entityManager.getMaskCategory(cam)) {
        Transform &transform = componentManager.getComponent<Transform>(id);
        Camera camera = componentManager.getComponent<Camera>(id);

        if (this->_graphic->getMouseMovement())
            this->_graphic->getCamera().rotate(transform, this->_graphic->getMouseOffset());
        this->_graphic->getCamera().update(transform, camera);
        this->_graphic->getCamera().setView(glfwGetTime());

        this->_graphic->getCamera().setDirLights(this->_dirLights);
        this->_graphic->getCamera().setPointLights(this->_pointLights);
    }
    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();
}
