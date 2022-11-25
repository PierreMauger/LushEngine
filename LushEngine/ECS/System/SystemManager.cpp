#include "ECS/System/SystemManager.hpp"

using namespace Lush;

SystemManager::SystemManager()
{
}

SystemManager::~SystemManager()
{
    for (auto &system : this->_systems)
        system.reset();
}

void SystemManager::bindSystem(std::unique_ptr<ISystem> system)
{
    this->_systems.push_back(std::move(system));
}

void SystemManager::updateSystems(EntityManager &entityManager, ComponentManager &componentManager)
{
    for (auto &system : this->_systems)
        system->update(entityManager, componentManager);
}
