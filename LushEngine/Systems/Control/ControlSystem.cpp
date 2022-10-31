#include "Systems/Control/ControlSystem.hpp"

using namespace Lush;

ControlSystem::ControlSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;

    this->_graphic->setMousePosition(glm::vec2(640, 360));
    glfwSetWindowUserPointer(this->_graphic->getWindow().get(), this);
    auto keyboardPressCallback = [](GLFWwindow *w, int key, int scan, int action, int mods) {
        static_cast<ControlSystem *>(glfwGetWindowUserPointer(w))->handleKeyboardPress(key, scan, action, mods);
    };
    glfwSetKeyCallback(this->_graphic->getWindow().get(), keyboardPressCallback);
}

void ControlSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    double x, y;
    glfwGetCursorPos(this->_graphic->getWindow().get(), &x, &y);
    if (this->_graphic->getMouseMovement())
        this->_graphic->setMouseOffset(glm::vec2(x, y));
    else
        this->_graphic->setMousePosition(glm::vec2(x, y));

    std::size_t controllable = (ComponentType::TRANSFORM | ComponentType::CONTROL);

    for (auto &id : entityManager.getMaskCategory(controllable)) {
        Transform &transform = componentManager.getComponent<Transform>(id);
        Control control = componentManager.getComponent<Control>(id);

        if (!control.control)
            continue;
        if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_W) == GLFW_PRESS)
            transform.position.x += 1;
        if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_S) == GLFW_PRESS)
            transform.position.x -= 1;
        if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_A) == GLFW_PRESS)
            transform.position.z += 1;
        if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_D) == GLFW_PRESS)
            transform.position.z -= 1;
        if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_SPACE) == GLFW_PRESS)
            transform.position.y += 1;
        if (glfwGetKey(this->_graphic->getWindow().get(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            transform.position.y -= 1;
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
