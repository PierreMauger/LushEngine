#include "System/Render/RenderSystem.hpp"

using namespace Lush;

RenderSystem::RenderSystem(std::shared_ptr<GLFWwindow> window)
{
    this->_window = window;
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
}
