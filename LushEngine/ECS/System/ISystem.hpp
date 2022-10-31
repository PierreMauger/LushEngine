#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "ECS/Component/ComponentManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "Includes.hpp"

namespace Lush
{
    class ISystem
    {
        public:
            virtual ~ISystem() = default;
            virtual void update(EntityManager &entityManager, ComponentManager &componentManager) = 0;
    };
}

#endif // ISYSTEM_HPP
