#include "Systems/Control/ControlSystem.hpp"

using namespace Lush;

ControlSystem::ControlSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager) : _graphic(graphic)
{
    entityManager.addMaskCategory(CONTROL_TAG);
    entityManager.addMaskCategory(CAMERA_TAG);
}

void ControlSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    for (auto id : entityManager.getMaskCategory(CONTROL_TAG)) {
        Transform &transform = componentManager.getComponent<Transform>(id);
        Control &control = componentManager.getComponent<Control>(id);

        if (!control.control)
            continue;
        for (auto cameraId : entityManager.getMaskCategory(CAMERA_TAG)) {
            Camera camera = componentManager.getComponent<Camera>(cameraId);

            if (camera.mod == CameraMod::THIRD_PERSON && camera.target == id) {
                glm::vec3 cameraNormal = glm::normalize(glm::vec3(camera.forward.x, 0.0f, camera.forward.z));

                control.alignTarget = false;
                if (glfwGetKey(this->_graphic->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
                    transform.position.x -= cameraNormal.z * 0.3f;
                    transform.position.z += cameraNormal.x * 0.3f;
                    control.alignTarget = true;
                }
                if (glfwGetKey(this->_graphic->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
                    transform.position.x += cameraNormal.z * 0.3f;
                    transform.position.z -= cameraNormal.x * 0.3f;
                    control.alignTarget = true;
                }
                if (glfwGetKey(this->_graphic->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
                    transform.position.x += cameraNormal.x * 0.3f;
                    transform.position.z += cameraNormal.z * 0.3f;
                    control.alignTarget = true;
                }
                if (glfwGetKey(this->_graphic->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
                    transform.position.x -= cameraNormal.x * 0.3f;
                    transform.position.z -= cameraNormal.z * 0.3f;
                    control.alignTarget = true;
                }
            }
        }
    }
}
