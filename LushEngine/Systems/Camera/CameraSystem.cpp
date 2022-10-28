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

            if (light.type == 0)
                this->_dirLights.push_back({transform, light});
            else if (light.type == 1)
                this->_pointLights.push_back({transform, light});
            else if (light.type == 2)
                this->_spotLights.push_back({transform, light});
        }
    }

    this->_graphic->getCamera().use("Camera");
    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & cam) == cam) {
            Transform &transform = componentManager.getComponent<Transform>(i);
            Camera camera = componentManager.getComponent<Camera>(i);

            if (this->_graphic->getMouseMovement())
                this->_graphic->getCamera().rotate(transform, this->_graphic->getMouseOffset());
            this->_graphic->getCamera().update(transform, camera);
            this->_graphic->getCamera().setView(glfwGetTime());

            this->_graphic->getCamera().setDirLights(this->_dirLights);
            this->_graphic->getCamera().setPointLights(this->_pointLights);
        }
    }
    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();
}
