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
}

void PhysicSystem::update([[maybe_unused]] EntityManager &entityManager, float deltaTime)
{
    if (this->_graphic->getPaused() || !this->_graphic->getRunning() || !this->shouldUpdate(deltaTime))
        return;
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<RigidBody>())
            continue;
        auto &transform = entity.getComponent<Transform>();
        auto &rigidBody = entity.getComponent<RigidBody>();

        btCollisionShape *collisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
        // btCollisionShape *collisionShape = new btSphereShape(1.0f);

        if (rigidBody.body == nullptr) {
            std::cerr << "Creating rigid body for entity " << id << std::endl;
            btTransform startTransform;
            startTransform.setIdentity();
            startTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
            btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);

            // Calculate the local inertia of the rigid body based on its mass and collision shape
            btVector3 localInertia(0, 0, 0);
            if (rigidBody.mass != 0.0f)
                collisionShape->calculateLocalInertia(rigidBody.mass, localInertia);

            // Create the rigid body using the motion state, collision shape, and local inertia
            btRigidBody::btRigidBodyConstructionInfo rbInfo(rigidBody.mass, motionState, collisionShape, localInertia);
            rigidBody.body = new btRigidBody(rbInfo);

            // Set additional properties of the rigid body (e.g., friction, restitution)
            ((btRigidBody *)rigidBody.body)->setFriction(rigidBody.friction);
            ((btRigidBody *)rigidBody.body)->setRestitution(rigidBody.restitution);
            ((btRigidBody *)rigidBody.body)->setLinearVelocity(btVector3(0, -1, 0));

            // Set the user pointer of the rigid body to the entity ID (optional)
            ((btRigidBody *)rigidBody.body)->setUserPointer(reinterpret_cast<void *>(id));
            // add the body to the dynamics world
            this->_dynamicsWorld->addRigidBody((btRigidBody *)rigidBody.body);
        }

        // Perform simulation step for the rigid body
        this->_dynamicsWorld->stepSimulation(deltaTime, 10);

        this->updateTransform((btRigidBody *)rigidBody.body, transform);
    }
}

void PhysicSystem::updateTransform(btRigidBody *rigidBody, Transform &transform)
{
    btTransform bulletTransform;
    rigidBody->getMotionState()->getWorldTransform(bulletTransform);
    btVector3 bulletPosition = bulletTransform.getOrigin();
    btQuaternion bulletRotation = bulletTransform.getRotation();

    transform.position = glm::vec3(bulletPosition.x(), bulletPosition.y(), bulletPosition.z());
    transform.rotation = glm::eulerAngles(glm::quat(bulletRotation.w(), bulletRotation.x(), bulletRotation.y(), bulletRotation.z()));
}
