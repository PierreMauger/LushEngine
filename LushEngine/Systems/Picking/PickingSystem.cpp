#include "Systems/Picking/PickingSystem.hpp"

using namespace Lush;

PickingSystem::PickingSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;
}

void PickingSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
}
