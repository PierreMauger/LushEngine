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
            std::vector<Resource> _scheduledDelete;

            void reloadResource(Resource &resource, std::shared_ptr<EntityManager> &entityManager);
            void deleteResource(Resource &resource);

            void reloadModel(Resource &resource);
            void reloadTexture(Resource &resource);
            void reloadSkybox(Resource &resource);
            void reloadShader(Resource &resource);
            void reloadScriptPack(Resource &resource, std::shared_ptr<EntityManager> &entityManager);
            void reloadScene(Resource &resource, std::shared_ptr<EntityManager> &entityManager);

            // void deleteScriptPack(Resource &resource, std::shared_ptr<EntityManager> &entityManager);
            // void deleteScene(Resource &resource, std::shared_ptr<EntityManager> &entityManager);

        public:
            FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~FileWatcherSystem() override = default;

            void update(std::shared_ptr<EntityManager> &entityManager, float deltaTime) override;
    };
}

#endif // FILE_WATCHER_SYSTEM_HPP
