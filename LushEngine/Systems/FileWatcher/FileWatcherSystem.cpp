#include "Systems/FileWatcher/FileWatcherSystem.hpp"

using namespace Lush;

FileWatcherSystem::FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(3.0f), _graphic(graphic), _resourceManager(resourceManager)
{
}

void FileWatcherSystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;

    // handle deleted files
    for (auto it = this->_resourceManager->getFiles().begin(); it != this->_resourceManager->getFiles().end();) {
        if (it->second.isDeleted()) {
            it = this->_resourceManager->getFiles().erase(it);
            this->deleteResourcesFromFile(it->second);
        } else
            ++it;
    }
    // handle modified files
    for (auto &[name, file] : this->_resourceManager->getFiles()) {
        if (file.isModified()) {
            file.updateLastModify();
            this->reloadResourcesFromFile(file, entityManager);
        }
    }

    // handle scheduled resources
    if (!this->_graphic->isRunning() && !this->_scheduledDelete.empty()) {
        for (auto &res : this->_scheduledDelete)
            this->deleteResource(res);
        this->_scheduledDelete.clear();
    }
    if (!this->_graphic->isRunning() && !this->_scheduledReload.empty()) {
        for (auto &res : this->_scheduledReload)
            this->reloadResource(res, entityManager);
        this->_scheduledReload.clear();
    }
}

void FileWatcherSystem::reloadResourcesFromFile(File &file, std::shared_ptr<EntityManager> &entityManager)
{
    for (auto &res : Resource::getResources())
        if (res.hasFile(file)) {
            std::cout << "[Toast Info]Reloading resource " << res.getUUID() << " for file " << file.getPath() << std::endl;
            this->reloadResource(res, entityManager);
        }
}

void FileWatcherSystem::deleteResourcesFromFile(File &file)
{
    for (auto &res : Resource::getResources())
        if (res.hasFile(file)) {
            std::cout << "[Toast Info]Deleting resource " << res.getUUID() << " for file " << file.getPath() << std::endl;
            this->deleteResource(res);
        }
}

void FileWatcherSystem::reloadResource(Resource &resource, std::shared_ptr<EntityManager> &entityManager)
{
    switch (resource.getType()) {
    case ResourceType::MODEL:
        this->reloadModel(resource);
        break;
    case ResourceType::TEXTURE:
        this->reloadTexture(resource);
        break;
    case ResourceType::SKYBOX:
        this->reloadSkybox(resource);
        break;
    case ResourceType::SHADER:
        this->reloadShader(resource);
        break;
    case ResourceType::SCRIPT:
        if (this->_graphic->isRunning()) {
            this->_scheduledReload.push_back(resource);
            std::cout << "[Toast Info]Scheduled reloading script " << resource.getUUID() << std::endl;
        } else
            this->reloadScriptPack(resource, entityManager);
        break;
    case ResourceType::SCENE:
        if (this->_graphic->isRunning()) {
            this->_scheduledReload.push_back(resource);
            std::cout << "[Toast Info]Scheduled reloading scene " << resource.getUUID() << std::endl;
        } else
            this->reloadScene(resource, entityManager);
        break;
    default:
        break;
    }
}

void FileWatcherSystem::deleteResource(Resource &resource)
{
    switch (resource.getType()) {
    case ResourceType::MODEL:
        this->_resourceManager->getModels().erase(resource.getUUID());
        break;
    case ResourceType::TEXTURE:
        this->_resourceManager->getTextures().erase(resource.getUUID());
        break;
    case ResourceType::SKYBOX:
        this->_resourceManager->getSkyboxes().erase(resource.getUUID());
        break;
    case ResourceType::SHADER:
        this->_resourceManager->getShaders().erase(resource.getUUID());
        break;
    case ResourceType::SCENE:
        if (this->_graphic->isRunning()) {
            this->_scheduledDelete.push_back(resource);
            std::cout << "[Toast Info]Scheduled deleting scene " << resource.getUUID() << std::endl;
        } else
            this->_resourceManager->getScenes().erase(resource.getUUID());
        break;
    default:
        break;
    }
}

void FileWatcherSystem::reloadModel(Resource &resource)
{
    auto it = std::ranges::find_if(this->_resourceManager->getModels(), [&resource](const auto &pair) { return pair.second == resource; });

    if (it == this->_resourceManager->getModels().end())
        return;
    try {
        it->second.reload(resource.getFiles()[0], this->_resourceManager->getTextures());
        std::cout << "[Toast Success]Reloaded model " << it->first << std::endl;
    } catch (const std::exception &e) {
        std::cout << "[Toast Error]" << e.what() << std::endl;
    }
}

void FileWatcherSystem::reloadTexture(Resource &resource)
{
    auto it = std::ranges::find_if(this->_resourceManager->getTextures(), [&resource](const auto &pair) { return pair.second == resource; });

    if (it == this->_resourceManager->getTextures().end())
        return;
    try {
        it->second.load(resource.getFiles()[0]);
        std::cout << "[Toast Success]Reloaded texture " << it->first << std::endl;
    } catch (const std::exception &e) {
        std::cout << "[Toast Error]" << e.what() << std::endl;
    }
}

void FileWatcherSystem::reloadSkybox(Resource &resource)
{
    auto it = std::ranges::find_if(this->_resourceManager->getSkyboxes(), [&resource](const auto &pair) { return pair.second == resource; });

    if (it == this->_resourceManager->getSkyboxes().end())
        return;
    try {
        it->second.load(resource.getFiles()[0]);
        std::cout << "[Toast Success]Reloaded skybox " << it->first << std::endl;
    } catch (const std::exception &e) {
        std::cout << "[Toast Error]" << e.what() << std::endl;
    }
}

void FileWatcherSystem::reloadShader(Resource &resource)
{
    auto it = std::ranges::find_if(this->_resourceManager->getShaders(), [&resource](const auto &pair) { return pair.second == resource; });

    if (it == this->_resourceManager->getShaders().end())
        return;
    try {
        it->second.reload(resource.getFiles()[0]);
        this->_graphic->getRenderView().setShaders(this->_resourceManager->getShaders());
        std::cout << "[Toast Success]Reloaded shader " << it->first << std::endl;
    } catch (const std::exception &e) {
        std::cout << "[Toast Error]" << e.what() << std::endl;
    }
}

void FileWatcherSystem::reloadScriptPack(Resource &resource, std::shared_ptr<EntityManager> &entityManager)
{
    if (*this->_resourceManager->getGamePack().get() != resource)
        return;

    std::vector<File> files = resource.getFiles();
    auto &scriptPack = this->_resourceManager->getGamePack();
    try {
        scriptPack->reload(files);
        std::cout << "[Toast Success]Reloaded Game script pack" << std::endl;
        for (auto &[className, klass] : scriptPack->getClasses()) {
            this->_resourceManager->getScripts()[className].reload(scriptPack->getDomain(), klass, this->_resourceManager->getEntityClass());

            for (auto &[id, entity] : entityManager->getEntities()) {
                if (!entity.hasScriptComponent(className))
                    continue;

                ScriptComponent scriptComponent(this->_resourceManager->getScripts()[className]);

                for (auto &[fieldName, field] : entity.getScriptComponent(className).getFields())
                    if (scriptComponent.hasField(fieldName))
                        scriptComponent.addField(fieldName, field);
                entity.addScriptComponent(className, scriptComponent);
            }
        }
    } catch (const std::exception &e) {
        std::cout << "[Toast Error]" << e.what() << std::endl;
    }
}

void FileWatcherSystem::reloadScene(Resource &resource, std::shared_ptr<EntityManager> &entityManager)
{
    auto it = std::ranges::find_if(this->_resourceManager->getScenes(), [&resource](const auto &pair) { return pair.second == resource; });

    if (it == this->_resourceManager->getScenes().end())
        return;
    try {
        it->second.reload(resource.getFiles()[0], this->_resourceManager->getScripts());
        std::cout << "[Toast Success]Reloaded scene " << it->first << std::endl;
    } catch (const std::exception &e) {
        std::cout << "[Toast Error]" << e.what() << std::endl;
    }
}
