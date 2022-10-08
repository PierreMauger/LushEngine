#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include "Component/ComponentManager.hpp"
#include "Entity/EntityManager.hpp"
#include "Includes.hpp"

namespace Lush
{
    class ISystem
    {
        public:
            virtual ~ISystem() = default;
            virtual void update(ComponentManager &componentManager, EntityManager &entityManager) = 0;
    };
}

#endif // ISYSTEM_HPP
