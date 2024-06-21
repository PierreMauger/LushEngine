#ifndef TERRAIN_INSTANCE_HPP
#define TERRAIN_INSTANCE_HPP

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <btBulletDynamicsCommon.h>

#include "ECS/Component/Component.hpp"
#include "Includes.hpp"
#include "Physic/BasicInstance.hpp"
#include "Rendering/Texture.hpp"

namespace Lush
{
    class TerrainInstance : public BasicInstance
    {
        private:
            btRigidBody *_rigidBody;

            // btDefaultMotionState *initTransform(Transform &transform);
            // btRigidBody *initRigidBody(RigidBody &rigidbody, btDefaultMotionState *motionState, btCollisionShape *collisionShape);

        public:
            TerrainInstance(std::size_t id, Transform &transform, Texture &texture);
            ~TerrainInstance() override;

            [[nodiscard]] btCollisionObject *getCollisionObject() const override;
            void preUpdate(Transform &transform, const Transform &parentTransform) override;
            void postUpdate(Transform &transform) override;
            void addToWorld(btDiscreteDynamicsWorld *world) override;
            void removeFromWorld(btDiscreteDynamicsWorld *world) override;

            [[nodiscard]] glm::vec3 getVelocity() const override;
            void setVelocity(const glm::vec3 &velocity) override;
            void addForce(const glm::vec3 &force) override;
    };
}

#endif // TERRAIN_INSTANCE_HPP
