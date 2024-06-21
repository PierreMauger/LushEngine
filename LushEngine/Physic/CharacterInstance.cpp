#include "Physic/CharacterInstance.hpp"

using namespace Lush;

CharacterInstance::CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController) : BasicInstance(id)
{
    btConvexShape *collisionShape = new btSphereShape(0);

    this->_ghostObject = new btPairCachingGhostObject();
    this->_ghostObject->setCollisionShape(collisionShape);
    this->_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    this->_ghostObject->setWorldTransform(startTransform);

    this->_characterController = new btKinematicCharacterController(this->_ghostObject, collisionShape, characterController.stepOffset);

    this->_characterController->setGravity(btVector3(0, -9.8, 0));
    this->_characterController->setStepHeight(characterController.stepOffset);
    this->_characterController->setMaxSlope(btRadians(characterController.slopeLimit));

    // btMotionState *motionState = new btDefaultMotionState(startTransform);
    // this->_rigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(1000, motionState, collisionShape));
    // this->_rigidBody->setCollisionFlags(this->_rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    // this->_rigidBody->setUserPointer(reinterpret_cast<void *>(id));
}

CharacterInstance::CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController, Collider &collider) : BasicInstance(id)
{
    btCollisionShape *collisionShape = this->initCollider(transform, collider);
    Collider c = collider;
    c.size += glm::vec3(0.1f, 0.0f, 0.0f);
    btCollisionShape *collisionShape2 = this->initCollider(transform, c);

    this->_ghostObject = new btPairCachingGhostObject();
    this->_ghostObject->setCollisionShape(collisionShape);
    this->_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    this->_collisionObject = new btPairCachingGhostObject();
    this->_collisionObject->setCollisionShape(collisionShape2);
    this->_collisionObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    this->_ghostObject->setWorldTransform(startTransform);
    this->_collisionObject->setWorldTransform(startTransform);

    this->_characterController = new btKinematicCharacterController(this->_ghostObject, dynamic_cast<btConvexShape *>(collisionShape), characterController.stepOffset);

    this->_characterController->setGravity(btVector3(0, -9.8, 0));
    this->_characterController->setStepHeight(characterController.stepOffset);
    this->_characterController->setMaxSlope(btRadians(characterController.slopeLimit));
}

CharacterInstance::~CharacterInstance()
{
    delete this->_characterController;
    delete this->_ghostObject->getCollisionShape();
    delete this->_ghostObject;
    delete this->_collisionObject->getCollisionShape();
    delete this->_collisionObject;
}

btCollisionObject *CharacterInstance::getCollisionObject() const
{
    return this->_ghostObject;
}

void CharacterInstance::preUpdate(Transform &transform, const Transform &parentTransform)
{
    btVector3 origin = this->_ghostObject->getWorldTransform().getOrigin();
    btVector3 diff = btVector3(transform.position.x, transform.position.y, transform.position.z) - origin;
    this->_characterController->setWalkDirection(diff);

    btTransform btTransform;
    btTransform.setIdentity();
    btTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    this->_collisionObject->setWorldTransform(btTransform);
}

void CharacterInstance::postUpdate(Transform &transform)
{
    btVector3 origin = this->_ghostObject->getWorldTransform().getOrigin();
    transform.position = glm::vec3(origin.x(), origin.y(), origin.z());
}

void CharacterInstance::addToWorld(btDiscreteDynamicsWorld *world)
{
    world->addCollisionObject(this->_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    world->addCollisionObject(this->_collisionObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    world->addAction(this->_characterController);
}

void CharacterInstance::removeFromWorld(btDiscreteDynamicsWorld *world)
{
    world->removeCollisionObject(this->_ghostObject);
    world->removeCollisionObject(this->_collisionObject);
    world->removeAction(this->_characterController);
}

glm::vec3 CharacterInstance::getVelocity() const
{
    btVector3 velocity = this->_characterController->getLinearVelocity();
    return glm::vec3(velocity.x(), velocity.y(), velocity.z());
}

void CharacterInstance::setVelocity(const glm::vec3 &velocity)
{
    this->_characterController->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

void CharacterInstance::addForce(const glm::vec3 &force)
{
    this->_characterController->jump(btVector3(force.x, force.y, force.z));
}
