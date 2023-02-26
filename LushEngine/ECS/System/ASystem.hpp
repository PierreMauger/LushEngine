#ifndef ASYSTEM_HPP
#define ASYSTEM_HPP

#include "ECS/Component/ComponentManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "Includes.hpp"

namespace Lush
{
    class ASystem
    {
        private:
            float _timeSinceLastUpdate = 0.0f;
            float _updateInterval = 0.0f;

        public:
            ASystem(float updateFrequency);
            virtual ~ASystem() = default;
            virtual void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime) = 0;

            bool shouldUpdate(float deltaTime);
            float getDeltaTime() const;
    };
}

#endif // ASYSTEM_HPP
