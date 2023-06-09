#ifndef GAME_SYSTEM_HPP
#define GAME_SYSTEM_HPP

#include "ECS/Component/Component.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"
#include "ResourceManager.hpp"

namespace Lush
{
    class GameSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            BufferObject _screen{};

            bool _started = false;

            void initInstances(EntityManager &entityManager);

        public:
            GameSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~GameSystem() override = default;

            void update(EntityManager &entityManager, float deltaTime) override;
    };
}

#endif // GAME_SYSTEM_HPP
