#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;
}

void CameraSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
    auto &masks = entityManager.getMasks();
    std::size_t cam = (ComponentType::TRANSFORM | ComponentType::CAMERA);
    std::size_t light = (ComponentType::TRANSFORM | ComponentType::LIGHT);

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

    this->_graphic->getCamera().use("Camera");
    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & cam) == cam) {
            Transform transform = componentManager.getComponent<Transform>(i);
            Camera camera = componentManager.getComponent<Camera>(i);

            this->_graphic->getCamera().update(transform, camera);
            this->_graphic->getCamera().setView(glfwGetTime());

            if (this->_dirLights.size() > 0)
                this->_graphic->getCamera().setDirLight(this->_dirLights[0]);
            this->_graphic->getCamera().setPointLights(this->_pointLights);
        }
    }
    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();
}
