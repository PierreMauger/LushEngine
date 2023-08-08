#include "Physic/CustomContactCallback.hpp"

using namespace Lush;

btScalar CustomContactCallback::addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0,
                                                const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1)
{
    const btCollisionObject *obj0 = colObj0Wrap->getCollisionObject();
    const btCollisionObject *obj1 = colObj1Wrap->getCollisionObject();

    if (this->_collisionTable.find({obj0, obj1}) == this->_collisionTable.end()) {
        this->_collisionTable[{obj0, obj1}] = COLLISION_ENTER;
        onCollision(obj0, obj1, COLLISION_ENTER);
    } else {
        this->_collisionTable[{obj0, obj1}] = COLLISION_STAY;
        onCollision(obj0, obj1, COLLISION_STAY);
    }
    return 0;
}

void CustomContactCallback::removeExitedCollisions()
{
    for (auto it = this->_collisionTable.begin(); it != this->_collisionTable.end();) {
        if (it->second == COLLISION_EXIT) {
            onCollision(it->first.first, it->first.second, COLLISION_EXIT);
            it = this->_collisionTable.erase(it);
        } else if (it->second == COLLISION_STAY) {
            it->second = COLLISION_EXIT;
            ++it;
        } else {
            ++it;
        }
    }
}

void CustomContactCallback::onCollision(const btCollisionObject *obj0, const btCollisionObject *obj1, CollisionState state)
{
    auto &physicInstances = ResourceManager::getStaticResourceManager()->getPhysicInstances();

    auto it0 = std::find_if(physicInstances.begin(), physicInstances.end(), [obj0](const auto &instance) { return instance.getRigidBody() == obj0; });
    auto it1 = std::find_if(physicInstances.begin(), physicInstances.end(), [obj1](const auto &instance) { return instance.getRigidBody() == obj1; });
    if (it0 == physicInstances.end() || it1 == physicInstances.end())
        return;

    auto &entity = ECS::getStaticEntityManager()->getEntity((*it0).getId());
    auto &scriptInstances = ResourceManager::getStaticResourceManager()->getScriptInstances();

    for (auto &[scriptName, index] : entity.getScriptIndexes()) {
        if (state == COLLISION_ENTER)
            scriptInstances[index].onCollisionEnter((*it1).getId());
        else if (state == COLLISION_STAY)
            scriptInstances[index].onCollisionStay((*it1).getId());
        else if (state == COLLISION_EXIT)
            scriptInstances[index].onCollisionExit((*it1).getId());
    }
}
