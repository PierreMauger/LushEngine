#include "Systems/Physic/PhysicSystem.hpp"

using namespace Lush;

class MyContactCallback : public btCollisionWorld::ContactResultCallback
{
    public:
        virtual btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper *colObj1Wrap,
                                         int partId1, int index1)
        {
            // Collision handling logic goes here
            // You can access the collision objects and other relevant information
            std::cerr << "Collision!" << std::endl;
            return 0;
        }
};

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

void PhysicSystem::update(EntityManager &entityManager, float deltaTime)
{
    if (this->_graphic->isPaused() || !this->_graphic->isRunning() || !this->shouldUpdate(deltaTime))
        return;

    for (auto &instance : this->_resourceManager->getCharacterInstances()) {
        auto &entity = entityManager.getEntity(instance.getId());
        auto &transform = entity.getComponent<Transform>();

        instance.preUpdate(transform);
    }
    this->_dynamicsWorld->stepSimulation(deltaTime);
    for (auto &instance : this->_resourceManager->getPhysicInstances()) {
        auto &entity = entityManager.getEntity(instance.getId());
        auto &transform = entity.getComponent<Transform>();

        instance.update(transform);
    }

    for (auto &instance : this->_resourceManager->getCharacterInstances()) {
        auto &entity = entityManager.getEntity(instance.getId());
        auto &transform = entity.getComponent<Transform>();

        instance.update(transform);
        // MyContactCallback callback;
        // this->_dynamicsWorld->contactTest(instance.getGhostObject(), callback);
    }
}
