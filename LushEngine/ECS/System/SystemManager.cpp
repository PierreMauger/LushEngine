#include "ECS/System/SystemManager.hpp"

using namespace Lush;

SystemManager::~SystemManager()
{
    for (auto &system : this->_systems)
        system.reset();
}

void SystemManager::updateSystems(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    for (auto &system : this->_systems)
        system->safeUpdate(entityManager, deltaTime);
}
