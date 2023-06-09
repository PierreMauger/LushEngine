#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

#include "ECS/System/ASystem.hpp"

namespace Lush
{
    class SystemManager
    {
        private:
            std::vector<std::unique_ptr<ASystem>> _systems;

        public:
            SystemManager() = default;
            ~SystemManager();

            void bindSystem(std::unique_ptr<ASystem> system);
            void updateSystems(EntityManager &entityManager, float deltaTime);
    };
}

#endif // SYSTEM_MANAGER_HPP
