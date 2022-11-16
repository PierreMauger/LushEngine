#include "Systems/Control/ControlSystem.hpp"

using namespace Lush;

ControlSystem::ControlSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;
    entityManager.addMaskCategory(this->_controlTag);
    entityManager.addMaskCategory(this->_cameraTag);

    this->_graphic->setMousePosition(glm::vec2(640, 360));
    glfwSetWindowUserPointer(this->_graphic->getWindow().get(), this);
    auto keyboardPressCallback = [](GLFWwindow *w, int key, int scan, int action, int mods) {
        static_cast<ControlSystem *>(glfwGetWindowUserPointer(w))->handleKeyboardPress(key, scan, action, mods);
    };
    glfwSetKeyCallback(this->_graphic->getWindow().get(), keyboardPressCallback);
}

void ControlSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    for (auto id : entityManager.getMaskCategory(this->_controlTag)) {
        Transform &transform = componentManager.getComponent<Transform>(id);
        Control &control = componentManager.getComponent<Control>(id);

        if (!control.control)
            continue;
        for (auto cameraId : entityManager.getMaskCategory(this->_cameraTag)) {
            Camera camera = componentManager.getComponent<Camera>(cameraId);

            if (camera.mod == CameraMod::THIRD_PERSON && camera.target == id) {
                glm::vec3 cameraNormal = glm::normalize(glm::vec3(camera.forward.x, 0.0f, camera.forward.z));

                control.alignTarget = false;
                if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_D) == GLFW_PRESS) {
                    transform.position.x -= cameraNormal.z * 0.3f;
                    transform.position.z += cameraNormal.x * 0.3f;
                    control.alignTarget = true;
                }
                if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_A) == GLFW_PRESS) {
                    transform.position.x += cameraNormal.z * 0.3f;
                    transform.position.z -= cameraNormal.x * 0.3f;
                    control.alignTarget = true;
                }
                if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_W) == GLFW_PRESS) {
                    transform.position.x += cameraNormal.x * 0.3f;
                    transform.position.z += cameraNormal.z * 0.3f;
                    control.alignTarget = true;
                }
                if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_S) == GLFW_PRESS) {
                    transform.position.x -= cameraNormal.x * 0.3f;
                    transform.position.z -= cameraNormal.z * 0.3f;
                    control.alignTarget = true;
                }
            }
        }
    }
}

void ControlSystem::handleKeyboardPress(int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(this->_graphic->getWindow().get(), true);
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        this->_graphic->setMouseMovement(!this->_graphic->getMouseMovement());
        glfwSetInputMode(this->_graphic->getWindow().get(), GLFW_CURSOR, this->_graphic->getMouseMovement() ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (this->_graphic->getMouseMovement()) {
            this->_graphic->setMousePosition(glm::vec2(640, 360));
            this->_graphic->setMouseOffset(glm::vec2(640, 360));
        }
        glfwSetCursorPos(this->_graphic->getWindow().get(), 640, 360);
    }
}
