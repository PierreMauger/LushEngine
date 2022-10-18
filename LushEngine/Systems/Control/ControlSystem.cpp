#include "Systems/Control/ControlSystem.hpp"

using namespace Lush;

ControlSystem::ControlSystem(std::shared_ptr<GLFWwindow> window)
{
    this->_window = window;
}

void ControlSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
}
