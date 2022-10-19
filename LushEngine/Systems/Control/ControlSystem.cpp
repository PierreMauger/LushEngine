#include "Systems/Control/ControlSystem.hpp"

using namespace Lush;

ControlSystem::ControlSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;
}

void ControlSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
    auto &masks = entityManager.getMasks();
    std::size_t controllable = (ComponentType::TRANSFORM | ComponentType::CONTROL);

    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & controllable) == controllable) {
            Transform &transform = componentManager.getComponent<Transform>(i);
            Control control = componentManager.getComponent<Control>(i);

            // if (glfwGetKey(this->_window.get(), GLFW_KEY_W) == GLFW_PRESS)
            //     transform.position += transform.rotation;
            // if (glfwGetKey(this->_window.get(), GLFW_KEY_S) == GLFW_PRESS)
            //     transform.position -= transform.rotation;
            // if (glfwGetKey(this->_window.get(), GLFW_KEY_A) == GLFW_PRESS)
            //     transform.position -= glm::normalize(glm::cross(transform.rotation, glm::vec3(0.0f, 1.0f, 0.0f)));
            // if (glfwGetKey(this->_window.get(), GLFW_KEY_D) == GLFW_PRESS)
            //     transform.position += glm::normalize(glm::cross(transform.rotation, glm::vec3(0.0f, 1.0f, 0.0f)));
            // if (glfwGetKey(this->_window.get(), GLFW_KEY_SPACE) == GLFW_PRESS)
            //     transform.position += glm::vec3(0.0f, 1.0f, 0.0f);
            // if (glfwGetKey(this->_window.get(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            //     transform.position -= glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}
