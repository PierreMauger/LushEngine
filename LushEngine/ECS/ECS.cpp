#include "ECS/ECS.hpp"

using namespace Lush;

static EntityManager *entityManager = nullptr;

EntityManager *ECS::getStaticEntityManager()
{
    return entityManager;
}

ECS::ECS()
{
    entityManager = &this->_entityManager;
}

EntityManager &ECS::getEntityManager()
{
    return this->_entityManager;
}

SystemManager &ECS::getSystemManager()
{
    return this->_systemManager;
}
