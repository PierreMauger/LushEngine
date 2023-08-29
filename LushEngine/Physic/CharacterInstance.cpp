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
    startTransform.setOrigin(btVector3(transform.position.x, transform.position.y - characterController.center.y, transform.position.z));
    this->_ghostObject->setWorldTransform(startTransform);

    this->_characterController = new btKinematicCharacterController(this->_ghostObject, collisionShape, characterController.stepOffset);

    this->_characterController->setGravity(btVector3(0, -9.8, 0));
    this->_characterController->setStepHeight(characterController.stepOffset);
    this->_characterController->setMaxSlope(btRadians(characterController.slopeLimit));
}

CharacterInstance::CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController, Collider &collider) : BasicInstance(id)
{
    btCollisionShape *collisionShape = this->initCollider(transform, collider);

    this->_ghostObject = new btPairCachingGhostObject();
    this->_ghostObject->setCollisionShape(collisionShape);
    this->_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(transform.position.x, transform.position.y - characterController.center.y, transform.position.z));
    this->_ghostObject->setWorldTransform(startTransform);

    this->_characterController = new btKinematicCharacterController(this->_ghostObject, dynamic_cast<btConvexShape *>(collisionShape), characterController.stepOffset);

    this->_characterController->setGravity(btVector3(0, -9.8, 0));
    this->_characterController->setStepHeight(characterController.stepOffset);
    this->_characterController->setMaxSlope(btRadians(characterController.slopeLimit));
}

CharacterInstance::~CharacterInstance()
{
    // delete this->_characterController;
    // delete this->_ghostObject->getCollisionShape();
    // delete this->_ghostObject;
}

btCollisionObject *CharacterInstance::getCollisionObject() const
{
    return this->_ghostObject;
}

void CharacterInstance::preUpdate(Transform &transform)
{
    btTransform btTransform;
    btTransform.setIdentity();
    btTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    this->_ghostObject->setWorldTransform(btTransform);
}

void CharacterInstance::postUpdate(Transform &transform)
{
    btVector3 origin = this->_ghostObject->getWorldTransform().getOrigin();
    transform.position = glm::vec3(origin.x(), origin.y(), origin.z());

    // if (this->_characterController->canJump() && this->_characterController->onGround()) {
    // this->_characterController->jump(btVector3(0, 5, 0));
    // }
}

void CharacterInstance::addToWorld(btDiscreteDynamicsWorld *world)
{
    world->addCollisionObject(this->_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    world->addAction(this->_characterController);
}

void CharacterInstance::removeFromWorld(btDiscreteDynamicsWorld *world)
{
    world->removeCollisionObject(this->_ghostObject);
    world->removeAction(this->_characterController);
}
