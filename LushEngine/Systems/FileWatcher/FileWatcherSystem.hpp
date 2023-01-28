#ifndef FILEWATCHERSYSTEM_HPP
#define FILEWATCHERSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"

namespace Lush
{
    class FileWatcherSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

        public:
            FileWatcherSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~FileWatcherSystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // FILEWATCHERSYSTEM_HPP
