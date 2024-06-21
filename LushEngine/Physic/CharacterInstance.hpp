#ifndef CHARACTER_INSTANCE_HPP
#define CHARACTER_INSTANCE_HPP

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <btBulletDynamicsCommon.h>

#include "ECS/Component/Component.hpp"
#include "Includes.hpp"
#include "Physic/BasicInstance.hpp"

namespace Lush
{
    class CharacterInstance : public BasicInstance
    {
        private:
            btKinematicCharacterController *_characterController;
            btPairCachingGhostObject *_ghostObject;
            btPairCachingGhostObject *_collisionObject;

        public:
            CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController);
            CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController, Collider &collider);
            ~CharacterInstance() override;

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

#endif // CHARACTER_INSTANCE_HPP
