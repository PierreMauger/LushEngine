#include "ECS/ECS.hpp"

using namespace Lush;

static ECS *ecs = nullptr;

ECS *ECS::getECS()
{
    return ecs;
}

ECS::ECS()
{
    ecs = this;
}

EntityManager &ECS::getEntityManager()
{
    return this->_entityManager;
}

SystemManager &ECS::getSystemManager()
{
    return this->_systemManager;
}
