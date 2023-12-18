#include "Systems/FileWatcher/FileWatcherSystem.hpp"

using namespace Lush;

FileWatcherSystem::FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(3.0f), _graphic(graphic), _resourceManager(resourceManager)
{
}

void FileWatcherSystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    // handle deleted files
    std::erase_if(this->_resourceManager->getFiles(), [this](const auto &entry) {
        if (entry.second.isDeleted()) {
            File file = entry.second;
            auto it = std::ranges::find_if(Resource::getResources(), [&file](const auto &res) { return res.hasFile(file); });
            if (it == Resource::getResources().end())
                return false;
            std::cout << "[Toast Info]Deleting " << file.getPath() << " (" << (*it).getUUID() << ")" << std::endl;
            this->deleteResource(*it);
            return true;
        }
        return false;
    });
    std::cout << Resource::getResources().size() << std::endl;
    // handle added files
    for (const auto &entry : std::filesystem::recursive_directory_iterator("Resources/Models")) {
        std::string path = entry.path().string();
        if (this->_resourceManager->getFiles().contains(path))
            continue;
        std::cout << "[Toast Info]Adding " << path << std::endl;
        File file(path);
        this->_resourceManager->getFiles()[path] = file;
        this->_resourceManager->getModels()[file.getName()] = RenderModel(file, this->_resourceManager->getTextures());
    }

    // handle modified files
    for (auto &[name, file] : this->_resourceManager->getFiles()) {
        if (file.isModified()) {
            file.updateLastModify();
            auto it = std::ranges::find_if(Resource::getResources(), [&file](const auto &res) { return res.hasFile(file); });
            if (it == Resource::getResources().end())
                continue;
            std::cout << "[Toast Info]Reloading " << file.getPath() << " (" << (*it).getUUID() << ")" << std::endl;
            this->reloadResource(*it, entityManager);
        }
    }

    // handle scheduled resources
    if (!this->_graphic->isRunning()) {
        if (!this->_scheduledDelete.empty()) {
            for (auto &res : this->_scheduledDelete)
                this->deleteResource(res);
            this->_scheduledDelete.clear();
        }
        if (!this->_scheduledReload.empty()) {
            for (auto &res : this->_scheduledReload)
                this->reloadResource(res, entityManager);
            this->_scheduledReload.clear();
        }
    }
}

void FileWatcherSystem::reloadResource(Resource &resource, std::shared_ptr<EntityManager> &entityManager)
{
    try {
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
    } catch (const std::exception &e) {
        std::cout << "[Toast Error]Failed to reload resource " << resource.getUUID() << ": " << e.what() << std::endl;
    }
}

void FileWatcherSystem::deleteResource(Resource &resource)
{
    switch (resource.getType()) {
    case ResourceType::MODEL:
        std::erase_if(this->_resourceManager->getModels(), [&resource](const auto &pair) { return pair.second == resource; });
        break;
    case ResourceType::TEXTURE:
        std::erase_if(this->_resourceManager->getTextures(), [&resource](const auto &pair) { return pair.second == resource; });
        break;
    case ResourceType::SKYBOX:
        std::erase_if(this->_resourceManager->getSkyboxes(), [&resource](const auto &pair) { return pair.second == resource; });
        break;
    case ResourceType::SHADER:
        std::erase_if(this->_resourceManager->getShaders(), [&resource](const auto &pair) { return pair.second == resource; });
        break;
    case ResourceType::SCRIPT:
        if (this->_graphic->isRunning()) {
            this->_scheduledDelete.push_back(resource);
            std::cout << "[Toast Info]Scheduled deleting script " << resource.getUUID() << std::endl;
        } else
            // TODO reload script pack
            this->_resourceManager->getScripts().erase(resource.getUUID());
        break;
    case ResourceType::SCENE:
        if (this->_graphic->isRunning()) {
            this->_scheduledDelete.push_back(resource);
            std::cout << "[Toast Info]Scheduled deleting scene " << resource.getUUID() << std::endl;
        } else
            // TODO test
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

    it->second.reload(resource.getFiles()[0], this->_resourceManager->getTextures());
    std::cout << "[Toast Success]Reloaded model " << it->first << std::endl;
}

void FileWatcherSystem::reloadTexture(Resource &resource)
{
    auto it = std::ranges::find_if(this->_resourceManager->getTextures(), [&resource](const auto &pair) { return pair.second == resource; });
    if (it == this->_resourceManager->getTextures().end())
        return;

    it->second.reload(resource.getFiles()[0]);
    std::cout << "[Toast Success]Reloaded texture " << it->first << std::endl;
}

void FileWatcherSystem::reloadSkybox(Resource &resource)
{
    auto it = std::ranges::find_if(this->_resourceManager->getSkyboxes(), [&resource](const auto &pair) { return pair.second == resource; });
    if (it == this->_resourceManager->getSkyboxes().end())
        return;

    it->second.load(resource.getFiles()[0]);
    std::cout << "[Toast Success]Reloaded skybox " << it->first << std::endl;
}

void FileWatcherSystem::reloadShader(Resource &resource)
{
    auto it = std::ranges::find_if(this->_resourceManager->getShaders(), [&resource](const auto &pair) { return pair.second == resource; });
    if (it == this->_resourceManager->getShaders().end())
        return;

    it->second.reload(resource.getFiles()[0]);
    this->_graphic->getRenderView().setShaders(this->_resourceManager->getShaders());
    std::cout << "[Toast Success]Reloaded shader " << it->first << std::endl;
}

void FileWatcherSystem::reloadScriptPack(Resource &resource, std::shared_ptr<EntityManager> &entityManager)
{
    if (*this->_resourceManager->getGamePack().get() != resource)
        return;
    std::vector<File> files = resource.getFiles();
    auto &scriptPack = this->_resourceManager->getGamePack();

    scriptPack->reload(files);
    std::cout << "[Toast Success]Reloaded Game script pack" << std::endl;
    for (auto &[className, klass] : scriptPack->getClasses()) {
        this->_resourceManager->getScripts()[className].reload(scriptPack->getDomain(), klass, this->_resourceManager->getComponentClass());

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
}

void FileWatcherSystem::reloadScene(Resource &resource, std::shared_ptr<EntityManager> &entityManager)
{
    auto it = std::ranges::find_if(this->_resourceManager->getScenes(), [&resource](const auto &pair) { return pair.second == resource; });
    if (it == this->_resourceManager->getScenes().end())
        return;

    it->second.reload(resource.getFiles()[0], this->_resourceManager->getScripts());
    std::cout << "[Toast Success]Reloaded scene " << it->first << std::endl;
}
