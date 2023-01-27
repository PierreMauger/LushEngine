#ifndef SYSTEMMANAGER_HPP
#define SYSTEMMANAGER_HPP

#include "ECS/System/ASystem.hpp"

namespace Lush
{
    class SystemManager
    {
        private:
            std::vector<std::unique_ptr<ASystem>> _systems;

        public:
            SystemManager();
            ~SystemManager();

            void bindSystem(std::unique_ptr<ASystem> system);
            void updateSystems(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // SYSTEMMANAGER_HPP
