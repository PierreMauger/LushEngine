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
}

void PhysicSystem::update(EntityManager &entityManager, float deltaTime)
{
    if (this->_graphic->getPaused() || !this->_graphic->getRunning() || !this->shouldUpdate(deltaTime))
        return;

    this->_dynamicsWorld->stepSimulation(deltaTime);
    for (auto &instance : this->_resourceManager->getPhysicInstances()) {
        auto &entity = entityManager.getEntity(instance.getId());
        auto &transform = entity.getComponent<Transform>();

        instance.update(transform);
    }
}
