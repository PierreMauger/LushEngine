#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;

    glfwMakeContextCurrent(this->_graphic->getWindow().get());
    glfwSetWindowUserPointer(this->_graphic->getWindow().get(), this);
    auto keyboardPressCallback = [](GLFWwindow *w, int key, int scan, int action, int mods) {
        static_cast<CameraSystem *>(glfwGetWindowUserPointer(w))->handleKeyboardPress(key, scan, action, mods);
    };
    glfwSetKeyCallback(this->_graphic->getWindow().get(), keyboardPressCallback);
}

void CameraSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
    if (this->_mouseMovement) {
        double x, y;
        glfwGetCursorPos(this->_graphic->getWindow().get(), &x, &y);
        glfwSetCursorPos(this->_graphic->getWindow().get(), 640, 360);
        this->_graphic->setMousePosition(glm::vec2(x, y));
    }

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
            Transform &transform = componentManager.getComponent<Transform>(i);
            Camera camera = componentManager.getComponent<Camera>(i);

            if (this->_mouseMovement)
                this->_graphic->getCamera().rotate(transform, this->_graphic->getMouseOffset());
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

void CameraSystem::handleKeyboardPress(int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(this->_graphic->getWindow().get(), true);
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        this->_mouseMovement = !this->_mouseMovement;
        glfwSetInputMode(this->_graphic->getWindow().get(), GLFW_CURSOR, this->_mouseMovement ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}
