#include "Systems/Physic/PhysicSystem.hpp"

using namespace Lush;

PhysicSystem::PhysicSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
}

void PhysicSystem::update([[maybe_unused]] EntityManager &entityManager, float deltaTime)
{
    if (this->_graphic->getPaused() || !this->_graphic->getRunning() || !this->shouldUpdate(deltaTime))
        return;
}
