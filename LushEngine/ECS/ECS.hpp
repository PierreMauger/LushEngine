#ifndef ECS_HPP
#define ECS_HPP

#include "ECS/Entity/EntityManager.hpp"
#include "ECS/System/SystemManager.hpp"

namespace Lush
{
    class ECS
    {
        private:
            std::shared_ptr<EntityManager> _entityManager;
            SystemManager _systemManager;

        public:
            ECS();
            ~ECS() = default;

            std::shared_ptr<EntityManager> &getEntityManager();
            SystemManager &getSystemManager();

            static std::shared_ptr<EntityManager> getStaticEntityManager();
    };
}

#endif // ECS_HPP
