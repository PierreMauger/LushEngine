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

    btStaticPlaneShape *planeShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);                                           // Plane normal (0, 1, 0) and distance 0
    btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -5, 0))); // No rotation, position at (0, 0, 0)
    btVector3 localInertia(0, 0, 0);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, planeShape, localInertia);
    btRigidBody *planeRigidBody = new btRigidBody(rbInfo);
    this->_dynamicsWorld->addRigidBody(planeRigidBody);

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
