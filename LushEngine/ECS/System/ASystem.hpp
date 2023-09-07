#ifndef A_SYSTEM_HPP
#define A_SYSTEM_HPP

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
            explicit ASystem(float updateFrequency);
            virtual ~ASystem() = default;

            virtual void update(std::shared_ptr<EntityManager> &entityManager, float deltaTime) = 0;

            bool shouldUpdate(float deltaTime);
            [[nodiscard]] float getDeltaTime() const;
    };
}

#endif // A_SYSTEM_HPP
