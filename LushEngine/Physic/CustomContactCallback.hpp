#ifndef CUSTOM_CONTACT_CALLBACK_HPP
#define CUSTOM_CONTACT_CALLBACK_HPP

#include <btBulletDynamicsCommon.h>

#include "Includes.hpp"
#include "ResourceManager.hpp"

namespace std
{
    template <> struct hash<std::pair<const btCollisionObject *, const btCollisionObject *>> {
            std::size_t operator()(const std::pair<const btCollisionObject *, const btCollisionObject *> &p) const
            {
                return hash<const btCollisionObject *>()(p.first) ^ hash<const btCollisionObject *>()(p.second);
            }
    };

    template <> struct equal_to<std::pair<const btCollisionObject *, const btCollisionObject *>> {
            bool operator()(const std::pair<const btCollisionObject *, const btCollisionObject *> &lhs,
                            const std::pair<const btCollisionObject *, const btCollisionObject *> &rhs) const
            {
                return lhs.first == rhs.first && lhs.second == rhs.second;
            }
    };
}

namespace Lush
{
    enum CollisionState {
        COLLISION_ENTER,
        COLLISION_STAY,
        COLLISION_EXIT
    };

    class CustomContactCallback : public btCollisionWorld::ContactResultCallback
    {
        private:
            std::unordered_map<std::pair<const btCollisionObject *, const btCollisionObject *>, CollisionState> _collisionTable;

        public:
            virtual btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper *colObj1Wrap,
                                             int partId1, int index1);

            void removeExitedCollisions();
            void onCollision(const btCollisionObject *obj0, const btCollisionObject *obj1, CollisionState state);
    };
}

#endif /* !CUSTOM_CONTACT_CALLBACK_HPP */
