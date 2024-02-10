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

            template <typename T, typename... Args> void bindSystem(Args &&...args)
            {
                this->_systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
            }

            void updateSystems(std::shared_ptr<EntityManager> &entityManager, float deltaTime);
    };
}

#endif // SYSTEM_MANAGER_HPP
