#include "ECS/System/SystemManager.hpp"

using namespace Lush;

SystemManager::~SystemManager()
{
    for (auto &system : this->_systems)
        system.reset();
}

void SystemManager::bindSystem(std::unique_ptr<ASystem> system)
{
    this->_systems.push_back(std::move(system));
}

void SystemManager::updateSystems(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    for (auto &system : this->_systems)
        system->update(entityManager, deltaTime);
}
