#ifndef PHYSIC_INSTANCE_HPP
#define PHYSIC_INSTANCE_HPP

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <btBulletDynamicsCommon.h>

#include "Physic/BasicInstance.hpp"

namespace Lush
{
    class PhysicInstance : public BasicInstance
    {
        private:
            btRigidBody *_rigidBody;

            btDefaultMotionState *initTransform(Transform &transform);
            btRigidBody *initRigidBody(RigidBody &rigidbody, btDefaultMotionState *motionState, btCollisionShape *collisionShape);

        public:
            PhysicInstance(std::size_t id, Transform &transform, RigidBody &rigidBody);
            PhysicInstance(std::size_t id, Transform &transform, Collider &collider);
            PhysicInstance(std::size_t id, Transform &transform, RigidBody &rigidBody, Collider &collider);
            ~PhysicInstance() override;

            btCollisionObject *getCollisionObject() const override;
            void preUpdate(Transform &transform) override;
            void postUpdate(Transform &transform) override;
            void addToWorld(btDiscreteDynamicsWorld *world) override;
            void removeFromWorld(btDiscreteDynamicsWorld *world) override;

            void updateRigidBodyRuntime(RigidBody &rigidBody);
            void updateColliderRuntime(Collider &collider);
    };
}

#endif // PHYSIC_INSTANCE_HPP
