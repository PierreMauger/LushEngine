#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;
    entityManager.addMaskCategory(this->_cameraTag);
    entityManager.addMaskCategory(this->_lightTag);
}

void CameraSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    for (auto id : entityManager.getMaskCategory(this->_lightTag)) {
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
    for (auto id : entityManager.getMaskCategory(this->_cameraTag)) {
        Transform &transform = componentManager.getComponent<Transform>(id);
        Camera &camera = componentManager.getComponent<Camera>(id);

        if (this->_graphic->getMouseMovement())
            this->_graphic->getCamera().rotate(transform, this->_graphic->getMouseOffset());
        camera.forward.x = cos(glm::radians(transform.rotation.x)) * cos(glm::radians(transform.rotation.y));
        camera.forward.y = sin(glm::radians(transform.rotation.y));
        camera.forward.z = sin(glm::radians(transform.rotation.x)) * cos(glm::radians(transform.rotation.y));

        if (camera.mod == CameraMod::THIRD_PERSON) {
            if (camera.target != id && entityManager.hasMask(camera.target, this->_transformTag)) {
                Transform &target = componentManager.getComponent<Transform>(camera.target);

                transform.position = target.position - camera.forward * camera.distance;
                if (camera.target != id && entityManager.hasMask(camera.target, this->_controlTag)) {
                    Control &control = componentManager.getComponent<Control>(camera.target);

                    if (control.forward != glm::vec3(0.0f) || camera.alignTarget)
                        target.rotation.y = glm::degrees(-atan2(control.forward.z, control.forward.x));
                }
            }
        }
        this->_graphic->getCamera().update(transform, camera);

        this->_graphic->getCamera().setDirLights(this->_dirLights);
        this->_graphic->getCamera().setPointLights(this->_pointLights);
    }
    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();
}
