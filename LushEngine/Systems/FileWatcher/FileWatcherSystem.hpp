#ifndef FILEWATCHERSYSTEM_HPP
#define FILEWATCHERSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "File/File.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"

namespace Lush
{
    class FileWatcherSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::vector<Resource> _resourcesToReload;

            void reloadResourcesFromFile(File &file);
            void updateResource(Resource &resource);

        public:
            FileWatcherSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~FileWatcherSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // FILEWATCHERSYSTEM_HPP
