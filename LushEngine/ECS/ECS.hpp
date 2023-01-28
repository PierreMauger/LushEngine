#ifndef ECS_HPP
#define ECS_HPP

#include "ECS/Component/ComponentManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "ECS/System/SystemManager.hpp"

namespace Lush
{
    class ECS
    {
        public:
            ECS();
            ~ECS() = default;

            EntityManager &getEntityManager();
            ComponentManager &getComponentManager();
            SystemManager &getSystemManager();

            static ECS *getECS();

        private:
            EntityManager _entityManager;
            ComponentManager _componentManager;
            SystemManager _systemManager;
    };
}

#endif // ECS_HPP
