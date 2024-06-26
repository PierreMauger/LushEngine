#include "Systems/Physic/PhysicSystem.hpp"

using namespace Lush;

PhysicSystem::PhysicSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(std::move(graphic)), _resourceManager(std::move(resourceManager))
{
    this->_broadphase = new btDbvtBroadphase();
    this->_collisionConfiguration = new btDefaultCollisionConfiguration();
    this->_dispatcher = new btCollisionDispatcher(this->_collisionConfiguration);
    this->_solver = new btSequentialImpulseConstraintSolver();
    this->_dynamicsWorld = new btDiscreteDynamicsWorld(this->_dispatcher, this->_broadphase, this->_solver, this->_collisionConfiguration);

    this->_dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
    this->_resourceManager->setDynamicsWorld(this->_dynamicsWorld);

    btGhostPairCallback *ghostPairCallback = new btGhostPairCallback();
    this->_dynamicsWorld->getPairCache()->setInternalGhostPairCallback(ghostPairCallback);
}

void PhysicSystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    if (this->_graphic->isPaused() || !this->_graphic->isRunning())
        return;

    for (auto &instance : this->_resourceManager->getPhysicInstances()) {
        if (!entityManager->hasEntity(instance->getId()))
            continue;
        Entity &entity = entityManager->getEntity(instance->getId());
        Transform &transform = entity.getComponent<Transform>();
        if (entity.getParent().has_value() && entityManager->getEntity(entity.getParent().value()).hasComponent<Transform>())
            instance->preUpdate(transform, entityManager->getEntity(entity.getParent().value()).getComponent<Transform>());
        else
            instance->preUpdate(transform);
    }
    this->_dynamicsWorld->stepSimulation(deltaTime);
    for (auto it = this->_resourceManager->getPhysicInstances().begin(); it != this->_resourceManager->getPhysicInstances().end();) {
        auto &instance = *it;
        if (!entityManager->hasEntity(instance->getId())) {
            instance->removeFromWorld(this->_resourceManager->getDynamicsWorld());
            it = this->_resourceManager->getPhysicInstances().erase(it);
            continue;
        }
        Transform &transform = entityManager->getEntity(instance->getId()).getComponent<Transform>();
        instance->postUpdate(transform);
        this->_dynamicsWorld->contactTest(instance->getCollisionObject(), this->_callback);
        ++it;
    }

    this->_callback.removeExitedCollisions();
}
