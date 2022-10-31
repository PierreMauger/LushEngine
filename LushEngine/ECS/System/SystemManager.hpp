#ifndef SYSTEMMANAGER_HPP
#define SYSTEMMANAGER_HPP

#include "ECS/System/ISystem.hpp"

namespace Lush
{
    class SystemManager
    {
        private:
            std::vector<std::shared_ptr<ISystem>> _systems;

        public:
            SystemManager();
            ~SystemManager();

            std::vector<std::shared_ptr<ISystem>> &getSystems();
            void bindSystem(std::shared_ptr<ISystem> system);
            void updateSystems(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // SYSTEMMANAGER_HPP
