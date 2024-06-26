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

void PhysicInstance::preUpdate(Transform &transform, const Transform &parentTransform)
{
    this->_rigidBody->setWorldTransform(btTransform(btQuaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w),
                                                    btVector3(transform.position.x, transform.position.y, transform.position.z)));

    this->_rigidBody->getMotionState()->setWorldTransform(
        btTransform(btQuaternion(parentTransform.rotation.x, parentTransform.rotation.y, parentTransform.rotation.z, parentTransform.rotation.w),
                    btVector3(parentTransform.position.x, parentTransform.position.y, parentTransform.position.z)));
}

void PhysicInstance::postUpdate(Transform &transform)
{
    btVector3 position = this->_rigidBody->getCenterOfMassPosition();
    btQuaternion quat = this->_rigidBody->getOrientation();

    transform.position = glm::vec3(position.x(), position.y(), position.z());
    transform.rotation = glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
}

void PhysicInstance::addToWorld(btDiscreteDynamicsWorld *world)
{
    world->addRigidBody(this->_rigidBody);
}

void PhysicInstance::removeFromWorld(btDiscreteDynamicsWorld *world)
{
    world->removeRigidBody(this->_rigidBody);
}

glm::vec3 PhysicInstance::getVelocity() const
{
    btVector3 velocity = this->_rigidBody->getLinearVelocity();
    return glm::vec3(velocity.x(), velocity.y(), velocity.z());
}

void PhysicInstance::setVelocity(const glm::vec3 &velocity)
{
    this->_rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

void PhysicInstance::addForce(const glm::vec3 &force)
{
    this->_rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
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
    startTransform.setRotation(btQuaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w));
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
