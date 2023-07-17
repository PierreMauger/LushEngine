#include "Physic/PhysicInstance.hpp"

using namespace Lush;

PhysicInstance::PhysicInstance(std::size_t id, Transform &transform, RigidBody &rigidBody)
{
    this->_id = id;

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    startTransform.setRotation(btQuaternion(btRadians(transform.rotation.y), btRadians(transform.rotation.x), btRadians(transform.rotation.z)));
    btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);

    btVector3 localInertia(0, 0, 0);
    btCollisionShape *collisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
    if (rigidBody.mass != 0.0f)
        collisionShape->calculateLocalInertia(rigidBody.mass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(rigidBody.mass, motionState, collisionShape, localInertia);
    this->_rigidBody = new btRigidBody(rbInfo);
    this->_rigidBody->setFriction(rigidBody.friction);
    this->_rigidBody->setRestitution(rigidBody.restitution);
    if (rigidBody.kinematic)
        this->_rigidBody->setCollisionFlags(this->_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

    this->_rigidBody->setUserPointer(reinterpret_cast<void *>(id));
}

std::size_t PhysicInstance::getId() const
{
    return this->_id;
}

btRigidBody *PhysicInstance::getRigidBody()
{
    return this->_rigidBody;
}

void PhysicInstance::update(Transform &transform)
{
    btVector3 position = this->_rigidBody->getCenterOfMassPosition();
    btQuaternion rotation = this->_rigidBody->getOrientation();
    btScalar roll, pitch, yaw;
    rotation.getEulerZYX(yaw, pitch, roll);

    roll = btDegrees(roll);
    pitch = btDegrees(pitch);
    yaw = btDegrees(yaw);

    transform.position = glm::vec3(position.x(), position.y(), position.z());
    transform.rotation = glm::vec3(roll, pitch, yaw);
}

void PhysicInstance::updateRigidBodyRuntime(RigidBody &rigidBody)
{
    this->_rigidBody->setFriction(rigidBody.friction);
    this->_rigidBody->setRestitution(rigidBody.restitution);

    btVector3 inertia(0, 0, 0);
    this->_rigidBody->getCollisionShape()->calculateLocalInertia(rigidBody.mass, inertia);
    this->_rigidBody->setMassProps(rigidBody.mass, inertia);

    if (rigidBody.kinematic)
        this->_rigidBody->setCollisionFlags(this->_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    else {
        this->_rigidBody->setCollisionFlags(this->_rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
        this->_rigidBody->setGravity(btVector3(0, -9.8, 0));
    }
}
