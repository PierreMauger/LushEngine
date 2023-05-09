#include "Systems/FileWatcher/FileWatcherSystem.hpp"

using namespace Lush;

FileWatcherSystem::FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(3.0f), _graphic(std::move(graphic)), _resourceManager(std::move(resourceManager))
{
}

void FileWatcherSystem::update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    // handle modified files
    for (auto &[name, file] : this->_resourceManager->getFiles()) {
        if (file.isModified()) {
            file.updateLastModify();
            this->reloadResourcesFromFile(file, entityManager, componentManager);
        }
    }
    // handle scheduled resources
    if (!this->_graphic->getRunning() && !this->_scheduledReload.empty()) {
        for (auto &res : this->_scheduledReload)
            this->updateResource(res, entityManager, componentManager);
        this->_scheduledReload.clear();
    }
}

void FileWatcherSystem::reloadResourcesFromFile(File &file, EntityManager &entityManager, ComponentManager &componentManager)
{
    for (auto &res : Resource::getResources())
        if (res.hasFile(file)) {
            std::cout << "Reloading resource " << res.getUUID() << " for file " << file.getPath() << std::endl;
            this->updateResource(res, entityManager, componentManager);
        }
}

void FileWatcherSystem::updateResource(Resource &resource, EntityManager &entityManager, ComponentManager &componentManager)
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
            this->reloadScriptPack(resource, componentManager);
        break;
    case ResourceType::SCENE:
        if (!handleScheduleReload(resource))
            this->reloadScene(resource, entityManager, componentManager);
        break;
    default:
        break;
    }
}

bool FileWatcherSystem::handleScheduleReload(Resource &resource)
{
    if (this->_graphic->getRunning()) {
        this->_scheduledReload.push_back(resource);
        std::cout << "Scheduled reloading scene " << resource.getUUID() << std::endl;
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
            std::cout << "Reloaded model " << name << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
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
            if (files.size() == 2)
                shader.reload(files[0], files[1]);
            else if (files.size() == 3)
                shader.reload(files[0], files[1], files[2]);
            else if (files.size() == 4)
                shader.reload(files[0], files[1], files[2], files[3]);
            else if (files.size() == 5)
                shader.reload(files[0], files[1], files[2], files[3], files[4]);
            this->_graphic->getRenderView().setShaders(this->_resourceManager->getShaders());
            std::cout << "Reloaded shader " << name << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        break;
    }
}

void FileWatcherSystem::reloadScriptPack(Resource &resource, ComponentManager &componentManager)
{
    std::vector<File> files = resource.getFiles();

    for (auto &[name, scriptPack] : this->_resourceManager->getScriptPacks()) {
        if (scriptPack != resource)
            continue;
        try {
            scriptPack.reload(files, this->_resourceManager->getScriptPacks());
            std::cout << "Reloaded script pack " << name << std::endl;
            if (name == "Core")
                continue;
            for (auto &[className, klass] : scriptPack.getClasses()) {
                this->_resourceManager->getScripts()[className].reload(scriptPack.getDomain(), klass, scriptPack.getEntityClass());

                SparseArray &sparseArray = componentManager.getAllInstanceFields(className);
                for (std::size_t i = 0; i < sparseArray.getSize(); i++) {
                    if (!sparseArray.getValues(i).has_value())
                        continue;
                    std::unordered_map<std::string, std::any> fieldsValues;
                    for (auto &[fieldName, field] : this->_resourceManager->getScripts()[className].getFields()) {
                        std::cout << "Adding field " << fieldName << " to " << className << std::endl;
                        if (field.type == "Single")
                            fieldsValues[fieldName] = 0.0f;
                        if (field.type == "Entity" || field.type == "UInt64")
                            fieldsValues[fieldName] = (unsigned long)0;
                    }
                    std::unordered_map<std::string, std::any> oldFieldsValues = std::any_cast<std::unordered_map<std::string, std::any> &>(sparseArray.getValues(i).value());
                    for (auto &[fieldName, oldFieldValue] : oldFieldsValues)
                        if (fieldsValues.find(fieldName) != fieldsValues.end())
                            fieldsValues[fieldName] = oldFieldValue;
                    componentManager.addInstanceFields(className, i, fieldsValues);
                }
            }
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        break;
    }
}

void FileWatcherSystem::reloadScene(Resource &resource, EntityManager &entityManager, ComponentManager &componentManager)
{
    std::cout << "Reloading scene " << resource.getUUID() << std::endl;
    for (auto &[name, scene] : this->_resourceManager->getScenes()) {
        if (scene != resource)
            continue;
        std::vector<File> files = resource.getFiles();
        scene.reload(files[0], this->_resourceManager->getScripts());
        scene.setScene(entityManager, componentManager);
        break;
    }
}
