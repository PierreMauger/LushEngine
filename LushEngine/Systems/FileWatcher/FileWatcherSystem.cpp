#include "Systems/FileWatcher/FileWatcherSystem.hpp"

using namespace Lush;

FileWatcherSystem::FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(3.0f), _graphic(graphic), _resourceManager(resourceManager)
{
}

void FileWatcherSystem::update(EntityManager &entityManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    // handle modified files
    for (auto &[name, file] : this->_resourceManager->getFiles()) {
        if (file.isModified()) {
            file.updateLastModify();
            this->reloadResourcesFromFile(file, entityManager);
        }
    }
    // handle scheduled resources
    if (!this->_graphic->isRunning() && !this->_scheduledReload.empty()) {
        for (auto &res : this->_scheduledReload)
            this->updateResource(res, entityManager);
        this->_scheduledReload.clear();
    }
}

void FileWatcherSystem::reloadResourcesFromFile(File &file, EntityManager &entityManager)
{
    for (auto &res : Resource::getResources())
        if (res.hasFile(file)) {
            std::cout << "[Toast Info]Reloading resource " << res.getUUID() << " for file " << file.getPath() << std::endl;
            this->updateResource(res, entityManager);
        }
}

void FileWatcherSystem::updateResource(Resource &resource, EntityManager &entityManager)
{
    switch (resource.getType()) {
    case ResourceType::MODEL:
        this->reloadModel(resource);
        break;
    case ResourceType::SHADER:
        this->reloadShader(resource);
        break;
    case ResourceType::SCRIPT:
        if (!handleScheduleReload(resource))
            this->reloadScriptPack(resource, entityManager);
        break;
    case ResourceType::SCENE:
        if (!handleScheduleReload(resource))
            this->reloadScene(resource, entityManager);
        break;
    default:
        break;
    }
}

bool FileWatcherSystem::handleScheduleReload(Resource &resource)
{
    if (this->_graphic->isRunning()) {
        this->_scheduledReload.push_back(resource);
        std::cout << "[Toast Info]Scheduled reloading scene " << resource.getUUID() << std::endl;
        return true;
    }
    return false;
}

void FileWatcherSystem::reloadModel(Resource &resource)
{
    std::vector<File> files = resource.getFiles();

    for (auto &[name, model] : this->_resourceManager->getModels()) {
        if (model != resource)
            continue;
        try {
            model.reload(files[0], this->_resourceManager->getTextures());
            std::cout << "[Toast Success]Reloaded model " << name << std::endl;
        } catch (const std::exception &e) {
            std::cout << "[Toast Error]" << e.what() << std::endl;
        }
        break;
    }
}

void FileWatcherSystem::reloadShader(Resource &resource)
{
    std::vector<File> files = resource.getFiles();

    for (auto &[name, shader] : this->_resourceManager->getShaders()) {
        if (shader != resource)
            continue;
        try {
            shader.reload(files[0]);
            this->_graphic->getRenderView().setShaders(this->_resourceManager->getShaders());
            std::cout << "[Toast Success]Reloaded shader " << name << std::endl;
        } catch (const std::exception &e) {
            std::cout << "[Toast Error]" << e.what() << std::endl;
        }
        break;
    }
}

void FileWatcherSystem::reloadScriptPack(Resource &resource, EntityManager &entityManager)
{
    std::vector<File> files = resource.getFiles();

    for (auto &[name, scriptPack] : this->_resourceManager->getScriptPacks()) {
        if (scriptPack != resource)
            continue;
        try {
            if (name == "Core")
                continue;
            scriptPack.reload(files);
            std::cout << "[Toast Success]Reloaded script pack " << name << std::endl;
            for (auto &[className, klass] : scriptPack.getClasses()) {
                this->_resourceManager->getScripts()[className].reload(scriptPack.getDomain(), klass, this->_resourceManager->getEntityClass());

                for (auto &[id, entity] : entityManager.getEntities()) {
                    if (!entity.hasScriptComponent(className))
                        continue;

                    ScriptComponent scriptComponent;
                    for (auto &[fieldName, field] : this->_resourceManager->getScripts()[className].getFields()) {
                        if (field.type == "Single")
                            scriptComponent.addField(fieldName, 0.0f);
                        if (field.type == "Entity" || field.type == "UInt64")
                            scriptComponent.addField(fieldName, (unsigned long)0);
                    }

                    ScriptComponent &oldScriptComponent = entity.getScriptComponent(className);
                    auto &oldFields = oldScriptComponent.getFields();
                    for (auto &[fieldName, field] : oldFields) {
                        if (scriptComponent.hasField(fieldName))
                            scriptComponent.addField(fieldName, field);
                    }
                    entity.addScriptComponent(className, scriptComponent);
                }
            }
        } catch (const std::exception &e) {
            std::cout << "[Toast Error]" << e.what() << std::endl;
        }
        break;
    }
}

void FileWatcherSystem::reloadScene(Resource &resource, EntityManager &entityManager)
{
    for (auto &[name, scene] : this->_resourceManager->getScenes()) {
        if (scene != resource)
            continue;
        std::vector<File> files = resource.getFiles();
        scene.reload(files[0], this->_resourceManager->getScripts());
        scene.setScene(entityManager);
        std::cout << "[Toast Success]Reloaded scene " << name << std::endl;
        break;
    }
}
