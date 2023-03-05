#ifndef FILEWATCHERSYSTEM_HPP
#define FILEWATCHERSYSTEM_HPP

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
            std::vector<Resource> _resourcesToReload;

            void reloadResourcesFromFile(File &file, ComponentManager &componentManager);
            void updateResource(Resource &resource, ComponentManager &componentManager);

            void reloadModel(Resource &resource);
            void reloadShader(Resource &resource);
            void reloadScriptPack(Resource &resource, ComponentManager &componentManager);
            void reloadScene(Resource &resource, ComponentManager &componentManager);

        public:
            FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~FileWatcherSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // FILEWATCHERSYSTEM_HPP
