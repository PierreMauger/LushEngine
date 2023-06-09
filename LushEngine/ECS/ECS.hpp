#ifndef ECS_HPP
#define ECS_HPP

#include "ECS/Entity/EntityManager.hpp"
#include "ECS/System/SystemManager.hpp"

namespace Lush
{
    class ECS
    {
        private:
            EntityManager _entityManager;
            SystemManager _systemManager;

        public:
            ECS();
            ~ECS() = default;

            EntityManager &getEntityManager();
            SystemManager &getSystemManager();

            static ECS *getECS();
    };
}

#endif // ECS_HPP
