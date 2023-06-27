#ifndef PHYSIC_INSTANCE_HPP
#define PHYSIC_INSTANCE_HPP

#include <btBulletDynamicsCommon.h>

#include "ECS/Component/Component.hpp"
#include "Includes.hpp"

namespace Lush
{
    class PhysicInstance
    {
        private:
            std::size_t _id;
            btRigidBody *_rigidBody;

        public:
            PhysicInstance(std::size_t id, Transform &transform, RigidBody &rigidBody);
            ~PhysicInstance() = default;

            btRigidBody *getRigidBody();
            std::size_t getId() const;

            void update(Transform &transform);

            void updateRigidBodyRuntime(RigidBody &rigidBody);
    };
}

#endif // PHYSIC_INSTANCE_HPP
