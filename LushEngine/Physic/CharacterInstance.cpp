#include "Physic/CharacterInstance.hpp"

using namespace Lush;

CharacterInstance::CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController)
{
    this->_id = id;

    auto capsuleShape = new btCapsuleShape(0.5f, 1.0f);

    this->_ghostObject = new btPairCachingGhostObject();
    this->_ghostObject->setCollisionShape(capsuleShape);
    this->_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    this->_characterController = new btKinematicCharacterController(this->_ghostObject, capsuleShape, characterController.stepOffset);

    btTransform btTransform;
    btTransform.setIdentity();
    btTransform.setOrigin(btVector3(transform.position.x, transform.position.y + characterController.center.y, transform.position.z));
    this->_ghostObject->setWorldTransform(btTransform);

    this->_characterController->setGravity(btVector3(0, -9.8, 0));
    // this->_characterController->setJumpSpeed(5.0f);
    // this->_characterController->setMaxJumpHeight(1.0f);
    // this->_characterController->setUseGhostSweepTest(true);
    // this->_characterController->setUpInterpolate(true);
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
