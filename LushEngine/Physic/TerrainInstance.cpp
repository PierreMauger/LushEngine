#include "Physic/TerrainInstance.hpp"

using namespace Lush;

TerrainInstance::TerrainInstance(std::size_t id, Transform &transform, Texture &texture) : BasicInstance(id)
{
    btHeightfieldTerrainShape *terrainShape =
        new btHeightfieldTerrainShape(texture.getWidth(), texture.getHeight(), texture.getHeightData(), 32.0f / 256.0f, 0.0f, 32.0f, 1, PHY_UCHAR, false);
    terrainShape->setUseDiamondSubdivision(true); // Enable diamond subdivision for better terrain smoothness
    terrainShape->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
    btDefaultMotionState *motionState = new btDefaultMotionState();
    this->_rigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, motionState, terrainShape));

    btTransform btTransform;
    btTransform.setIdentity();
    btTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    glm::quat rotation = glm::quat(glm::radians(transform.rotation));
    btTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
    this->_rigidBody->setWorldTransform(btTransform);
}

TerrainInstance::~TerrainInstance()
{
    delete this->_rigidBody->getMotionState();
    delete this->_rigidBody->getCollisionShape();
    delete this->_rigidBody;
}

btCollisionObject *TerrainInstance::getCollisionObject() const
{
    return this->_rigidBody;
}

void TerrainInstance::preUpdate(Transform &transform)
{
}

void TerrainInstance::postUpdate(Transform &transform)
{
}

void TerrainInstance::addToWorld(btDiscreteDynamicsWorld *world)
{
    world->addRigidBody(this->_rigidBody);
}

void TerrainInstance::removeFromWorld(btDiscreteDynamicsWorld *world)
{
    world->removeRigidBody(this->_rigidBody);
}
