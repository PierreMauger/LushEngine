#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic) : ASystem(60.0f), _graphic(graphic)
{
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

        if (this->_graphic->getMouseMovement()) {
            glm::vec2 offset = this->_graphic->getMouseOffset();
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::normalize(glm::cross(up, camera.forward));
            glm::quat q;
            q = glm::angleAxis(glm::radians(-offset.x) * camera.sensitivity, up);
            camera.forward = glm::normalize(q * camera.forward);
            q = glm::angleAxis(glm::radians(-offset.y) * camera.sensitivity, right);
            glm::vec3 forward = glm::normalize(q * camera.forward);
            if (glm::dot(forward, up) < 0.99f && glm::dot(forward, up) > -0.99f)
                camera.forward = forward;
            transform.rotation = glm::degrees(glm::eulerAngles(glm::quatLookAt(camera.forward, up)));
        }

        if (entityManager.hasMask(4, CONTROL_TAG)) {
            Transform &target = componentManager.getComponent<Transform>(4);

            transform.position = target.position - camera.forward * 10.0f;
        }
        glm::quat quaternion = glm::quat(glm::radians(transform.rotation));
        camera.forward = glm::normalize(quaternion * glm::vec3(0.0f, 0.0f, -1.0f));
        this->_graphic->getRenderView().update(transform, camera);

        this->_graphic->getRenderView().setDirLights(this->_dirLights);
        this->_graphic->getRenderView().setPointLights(this->_pointLights);
    }
    this->_graphic->getRenderView().use("Map");
    this->_graphic->getRenderView().setDirLights(this->_dirLights);
    this->_graphic->getRenderView().setPointLights(this->_pointLights);

    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();
}
