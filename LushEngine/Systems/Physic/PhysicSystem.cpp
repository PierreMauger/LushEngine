#include "Systems/Physic/PhysicSystem.hpp"

using namespace Lush;

PhysicSystem::PhysicSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
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
    if (this->_graphic->isPaused() || !this->_graphic->isRunning() || !this->shouldUpdate(deltaTime))
        return;

    for (auto &instance : this->_resourceManager->getPhysicInstances()) {
        auto &transform = entityManager->getEntity(instance->getId()).getComponent<Transform>();
        instance->preUpdate(transform);
    }
    this->_dynamicsWorld->stepSimulation(deltaTime);
    for (auto &instance : this->_resourceManager->getPhysicInstances()) {
        auto &transform = entityManager->getEntity(instance->getId()).getComponent<Transform>();
        instance->postUpdate(transform);
        this->_dynamicsWorld->contactTest(instance->getCollisionObject(), this->_callback);
    }

    this->_callback.removeExitedCollisions();
}
