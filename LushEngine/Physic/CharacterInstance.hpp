#ifndef CHARACTER_INSTANCE_HPP
#define CHARACTER_INSTANCE_HPP

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <btBulletDynamicsCommon.h>

#include "ECS/Component/Component.hpp"
#include "Includes.hpp"
#include "Rendering/Texture.hpp"

namespace Lush
{
    class CharacterInstance
    {
        private:
            std::size_t _id;
            btKinematicCharacterController *_characterController;
            btPairCachingGhostObject *_ghostObject;

        public:
            CharacterInstance(std::size_t id, Transform &transform, CharacterController &characterController);
            ~CharacterInstance() = default;

            std::size_t getId() const;
            btKinematicCharacterController *getCharacterController();
            btPairCachingGhostObject *getGhostObject() const;

            void preUpdate(Transform &transform);
            void update(Transform &transform);
    };
}

#endif // CHARACTER_INSTANCE_HPP
