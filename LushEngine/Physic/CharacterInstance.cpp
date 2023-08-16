#include "Physic/CharacterInstance.hpp"

using namespace Lush;

CharacterInstance::CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController)
{
    this->_id = id;

    btConvexShape *collisionShape = new btSphereShape(0);
    this->_ghostObject = new btPairCachingGhostObject();
    this->_ghostObject->setCollisionShape(collisionShape);
    this->_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(transform.position.x, transform.position.y + characterController.center.y, transform.position.z));
    this->_ghostObject->setWorldTransform(startTransform);

    this->_characterController = new btKinematicCharacterController(this->_ghostObject, collisionShape, characterController.stepOffset);

    this->_characterController->setGravity(btVector3(0, -9.8, 0));
    this->_characterController->setStepHeight(characterController.stepOffset);
    this->_characterController->setMaxSlope(btRadians(characterController.slopeLimit));
}

CharacterInstance::CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController, Collider &collider)
{
    this->_id = id;

    btConvexShape *collisionShape = nullptr;
    switch (collider.type) {
    case ColliderType::BOX:
        collisionShape = new btBoxShape(btVector3(collider.size.x, collider.size.y, collider.size.z));
        break;
    case ColliderType::SPHERE:
        collisionShape = new btSphereShape(collider.size.x);
        break;
    case ColliderType::CAPSULE:
        collisionShape = new btCapsuleShape(collider.size.x, collider.size.y);
        break;
    default:
        collisionShape = new btCapsuleShape(collider.size.x, collider.size.y);
        break;
    }

    this->_ghostObject = new btPairCachingGhostObject();
    this->_ghostObject->setCollisionShape(collisionShape);
    this->_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(transform.position.x, transform.position.y + characterController.center.y, transform.position.z));
    this->_ghostObject->setWorldTransform(startTransform);

    this->_characterController = new btKinematicCharacterController(this->_ghostObject, collisionShape, characterController.stepOffset);

    this->_characterController->setGravity(btVector3(0, -9.8, 0));
    this->_characterController->setStepHeight(characterController.stepOffset);
    this->_characterController->setMaxSlope(btRadians(characterController.slopeLimit));
}

std::size_t CharacterInstance::getId() const
{
    return this->_id;
}

btKinematicCharacterController *CharacterInstance::getCharacterController()
{
    return this->_characterController;
}

btPairCachingGhostObject *CharacterInstance::getGhostObject() const
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
