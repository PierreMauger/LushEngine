#ifndef FILE_WATCHER_SYSTEM_HPP
#define FILE_WATCHER_SYSTEM_HPP

#include "ECS/Component/Component.hpp"
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

            void reloadResourcesFromFile(File &file, EntityManager &entityManager);
            void updateResource(Resource &resource, EntityManager &entityManager);
            bool handleScheduleReload(Resource &resource);

            void reloadModel(Resource &resource);
            void reloadShader(Resource &resource);
            void reloadScriptPack(Resource &resource, EntityManager &entityManager);
            void reloadScene(Resource &resource, EntityManager &entityManager);

        public:
            FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~FileWatcherSystem() override = default;

            void update(EntityManager &entityManager, float deltaTime) override;
    };
}

#endif // FILE_WATCHER_SYSTEM_HPP
