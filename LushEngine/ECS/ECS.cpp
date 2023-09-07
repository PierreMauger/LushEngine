#include "ECS/ECS.hpp"

using namespace Lush;

static std::shared_ptr<EntityManager> *entityManager = nullptr;

std::shared_ptr<EntityManager> ECS::getStaticEntityManager()
{
    return *entityManager;
}

ECS::ECS()
{
    this->_entityManager = std::make_shared<EntityManager>();
    entityManager = &this->_entityManager;
}

std::shared_ptr<EntityManager> &ECS::getEntityManager()
{
    return this->_entityManager;
}

SystemManager &ECS::getSystemManager()
{
    return this->_systemManager;
}
