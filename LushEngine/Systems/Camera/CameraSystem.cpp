#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager) : ASystem(60.0f), _graphic(graphic)
{
    entityManager.addMaskCategory(CAMERA_TAG);
    entityManager.addMaskCategory(LIGHT_TAG);
}

void CameraSystem::update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    for (auto id : entityManager.getMaskCategory(LIGHT_TAG)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Light light = componentManager.getComponent<Light>(id);

        if (light.type == LightType::DIRECTIONAL)
            this->_dirLights.push_back({transform, light});
        else if (light.type == LightType::POINT)
            this->_pointLights.push_back({transform, light});
        else if (light.type == LightType::SPOT)
            this->_spotLights.push_back({transform, light});
    }

    this->_graphic->getRenderView().use("Model");
    for (auto id : entityManager.getMaskCategory(CAMERA_TAG)) {
        Transform &transform = componentManager.getComponent<Transform>(id);
        Camera &camera = componentManager.getComponent<Camera>(id);

        if (this->_graphic->getMouseMovement())
            this->_graphic->getRenderView().rotate(transform, this->_graphic->getMouseOffset());

        camera.forward.x = cos(glm::radians(transform.rotation.x)) * cos(glm::radians(transform.rotation.y));
        camera.forward.y = sin(glm::radians(transform.rotation.y));
        camera.forward.z = sin(glm::radians(transform.rotation.x)) * cos(glm::radians(transform.rotation.y));

        if (camera.mod == CameraMod::THIRD_PERSON && camera.target != id && entityManager.hasMask(camera.target, CONTROL_TAG)) {
            Transform &target = componentManager.getComponent<Transform>(camera.target);

            transform.position = target.position - camera.forward * camera.distance;
        }
        this->_graphic->getRenderView().update(transform, camera);

        this->_graphic->getRenderView().setDirLights(this->_dirLights);
        this->_graphic->getRenderView().setPointLights(this->_pointLights);
    }
    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();
}
