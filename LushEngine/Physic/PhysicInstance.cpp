#include "Physic/PhysicInstance.hpp"

using namespace Lush;

PhysicInstance::PhysicInstance(std::size_t id, Transform &transform, RigidBody &rigidBody) : BasicInstance(id)
{
    btDefaultMotionState *motionState = this->initTransform(transform);
    btCollisionShape *collisionShape = new btSphereShape(0);
    this->_rigidBody = this->initRigidBody(rigidBody, motionState, collisionShape);

    this->_rigidBody->setUserPointer(reinterpret_cast<void *>(id));
}

PhysicInstance::PhysicInstance(std::size_t id, Transform &transform, Collider &collider) : BasicInstance(id)
{
    btDefaultMotionState *motionState = this->initTransform(transform);
    btCollisionShape *collisionShape = this->initCollider(transform, collider);
    this->_rigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0.0f, motionState, collisionShape));
    this->_rigidBody->setCollisionFlags(this->_rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

    this->_rigidBody->setUserPointer(reinterpret_cast<void *>(id));
}

PhysicInstance::PhysicInstance(std::size_t id, Transform &transform, RigidBody &rigidBody, Collider &collider) : BasicInstance(id)
{
    btDefaultMotionState *motionState = this->initTransform(transform);
    btCollisionShape *collisionShape = this->initCollider(transform, collider);
    this->_rigidBody = this->initRigidBody(rigidBody, motionState, collisionShape);

    this->_rigidBody->setUserPointer(reinterpret_cast<void *>(id));
}

PhysicInstance::~PhysicInstance()
{
    delete this->_rigidBody->getMotionState();
    delete this->_rigidBody->getCollisionShape();
    delete this->_rigidBody;
}

btCollisionObject *PhysicInstance::getCollisionObject() const
{
    return this->_rigidBody;
}

void PhysicInstance::preUpdate(Transform &transform)
{
    bool diffPosition = false;
    bool diffRotation = false;

    btVector3 position = this->_rigidBody->getCenterOfMassPosition();
    btScalar roll, pitch, yaw;
    this->_rigidBody->getOrientation().getEulerZYX(yaw, pitch, roll);
    roll = btDegrees(roll);
    pitch = btDegrees(pitch);
    yaw = btDegrees(yaw);

    if (position != btVector3(transform.position.x, transform.position.y, transform.position.z))
        diffPosition = true;
    if (roll != transform.rotation.x || pitch != transform.rotation.y || yaw != transform.rotation.z)
        diffRotation = true;

    if (diffPosition || diffRotation) {
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
        btQuaternion rotation;
        rotation.setEulerZYX(btRadians(transform.rotation.z), btRadians(transform.rotation.y), btRadians(transform.rotation.x));
        startTransform.setRotation(rotation);
        this->_rigidBody->setWorldTransform(startTransform);
    }
}

void PhysicInstance::postUpdate(Transform &transform)
{
    btVector3 position = this->_rigidBody->getCenterOfMassPosition();
    btScalar roll, pitch, yaw;
    this->_rigidBody->getOrientation().getEulerZYX(yaw, pitch, roll);

    roll = btDegrees(roll);
    pitch = btDegrees(pitch);
    yaw = btDegrees(yaw);

    transform.position = glm::vec3(position.x(), position.y(), position.z());
    transform.rotation = glm::vec3(roll, pitch, yaw);
}

void PhysicInstance::addToWorld(btDiscreteDynamicsWorld *world)
{
    world->addRigidBody(this->_rigidBody);
}

void PhysicInstance::removeFromWorld(btDiscreteDynamicsWorld *world)
{
    world->removeRigidBody(this->_rigidBody);
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

void PhysicInstance::updateColliderRuntime(Collider &collider)
{
    std::cout << "updateColliderRuntime" << std::endl;
}

btDefaultMotionState *PhysicInstance::initTransform(Transform &transform)
{
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    startTransform.setRotation(btQuaternion(btRadians(transform.rotation.y), btRadians(transform.rotation.x), btRadians(transform.rotation.z)));
    return new btDefaultMotionState(startTransform);
}

btRigidBody *PhysicInstance::initRigidBody(RigidBody &rigidbody, btDefaultMotionState *motionState, btCollisionShape *collisionShape)
{
    btVector3 localInertia(0, 0, 0);
    if (rigidbody.mass != 0.0f)
        collisionShape->calculateLocalInertia(rigidbody.mass, localInertia);

    btRigidBody *rigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(rigidbody.mass, motionState, collisionShape, localInertia));
    rigidBody->setFriction(rigidbody.friction);
    rigidBody->setRestitution(rigidbody.restitution);
    if (rigidbody.kinematic)
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

    return rigidBody;
}
