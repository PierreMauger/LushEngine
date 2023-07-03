#ifndef PHYSIC_SYSTEM_HPP
#define PHYSIC_SYSTEM_HPP

#include <btBulletDynamicsCommon.h>

#include "ECS/Component/Component.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "ResourceManager.hpp"

namespace Lush
{
    class PhysicSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            btBroadphaseInterface *_broadphase;
            btDefaultCollisionConfiguration *_collisionConfiguration;
            btCollisionDispatcher *_dispatcher;
            btSequentialImpulseConstraintSolver *_solver;
            btDiscreteDynamicsWorld *_dynamicsWorld;

        public:
            explicit PhysicSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~PhysicSystem() override = default;

            void update(EntityManager &entityManager, float deltaTime) override;
    };
}

#endif // PHYSIC_SYSTEM_HPP
