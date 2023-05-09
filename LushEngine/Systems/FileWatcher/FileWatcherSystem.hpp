#ifndef FILE_WATCHER_SYSTEM_HPP
#define FILE_WATCHER_SYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "File/File.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "ResourceManager.hpp"

namespace Lush
{
    class FileWatcherSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            std::vector<Resource> _scheduledReload;

            void reloadResourcesFromFile(File &file, EntityManager &entityManager, ComponentManager &componentManager);
            void updateResource(Resource &resource, EntityManager &entityManager, ComponentManager &componentManager);
            bool handleScheduleReload(Resource &resource);

            void reloadModel(Resource &resource);
            void reloadShader(Resource &resource);
            void reloadScriptPack(Resource &resource, ComponentManager &componentManager);
            void reloadScene(Resource &resource, EntityManager &entityManager, ComponentManager &componentManager);

        public:
            FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~FileWatcherSystem() override = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime) override;
    };
}

#endif // FILE_WATCHER_SYSTEM_HPP
