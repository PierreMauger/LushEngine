#include "Physic/CustomContactCallback.hpp"

using namespace Lush;

btScalar CustomContactCallback::addSingleResult([[maybe_unused]] btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, [[maybe_unused]] int partId0,
                                                [[maybe_unused]] int index0, const btCollisionObjectWrapper *colObj1Wrap, [[maybe_unused]] int partId1, [[maybe_unused]] int index1)
{
    const btCollisionObject *obj0 = colObj0Wrap->getCollisionObject();
    const btCollisionObject *obj1 = colObj1Wrap->getCollisionObject();

    if (!this->_collisionTable.contains({obj0, obj1})) {
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

static std::size_t getEntityFromCollisionObject(const btCollisionObject *obj)
{
    auto &physicInstances = ResourceManager::getStaticResourceManager()->getPhysicInstances();
    auto physicIt = std::find_if(physicInstances.begin(), physicInstances.end(), [obj](const auto &instance) { return instance->getCollisionObject() == obj; });
    if (physicIt != physicInstances.end())
        return (*physicIt)->getId();

    return (std::size_t)-1;
}

void CustomContactCallback::onCollision(const btCollisionObject *obj0, const btCollisionObject *obj1, CollisionState state)
{
    auto id0 = getEntityFromCollisionObject(obj0);
    auto id1 = getEntityFromCollisionObject(obj1);

    if (id0 == (std::size_t)-1 || id1 == (std::size_t)-1)
        return;

    auto &entity0 = ECS::getStaticEntityManager()->getEntity(id0);
    auto &scriptInstances = ResourceManager::getStaticResourceManager()->getScriptInstances();

    for (auto &[scriptName, index] : entity0.getScriptIndexes()) {
        if (state == COLLISION_ENTER)
            scriptInstances[index].onCollisionEnter(id1);
        else if (state == COLLISION_STAY)
            scriptInstances[index].onCollisionStay(id1);
        else if (state == COLLISION_EXIT)
            scriptInstances[index].onCollisionExit(id1);
    }
}
