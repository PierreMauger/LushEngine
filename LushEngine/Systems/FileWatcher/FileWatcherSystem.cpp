#include "Systems/FileWatcher/FileWatcherSystem.hpp"

using namespace Lush;

FileWatcherSystem::FileWatcherSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(3.0f), _graphic(graphic), _resourceManager(resourceManager)
{
}

void FileWatcherSystem::update([[maybe_unused]] EntityManager &entityManager, ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    for (auto &[name, file] : this->_resourceManager->getFiles()) {
        if (file.isModified()) {
            file.update();
            this->reloadResourcesFromFile(file, componentManager);
        }
    }
    if (!this->_graphic->getRunning() && !this->_resourcesToReload.empty()) {
        for (auto &res : this->_resourcesToReload)
            this->updateResource(res, componentManager);
        this->_resourcesToReload.clear();
    }
}

void FileWatcherSystem::reloadResourcesFromFile(File &file, ComponentManager &componentManager)
{
    for (auto &res : Resource::getResources())
        if (res.hasFile(file)) {
            std::cout << "Reloading resource " << res.getUUID() << " for file " << file.getPath() << std::endl;
            this->updateResource(res, componentManager);
        }
}

void FileWatcherSystem::updateResource(Resource &resource, ComponentManager &componentManager)
{
    std::vector<File> files = resource.getFiles();

    switch (resource.getType()) {
    case ResourceType::MODEL:
        for (auto &[name, model] : this->_resourceManager->getModels()) {
            if (model != resource)
                continue;
            try {
                model.reload(files[0], this->_resourceManager->getTextures());
                std::cout << "Reloaded model " << name << std::endl;
            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
        }
        break;
    case ResourceType::SHADER:
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
        }
        break;
    case ResourceType::SCRIPT:
        if (this->_graphic->getRunning()) {
            this->_resourcesToReload.push_back(resource);
            std::cout << "Scheduled reloading script " << resource.getUUID() << std::endl;
            break;
        }
        try {
            for (auto &[name, scriptPack] : this->_resourceManager->getScriptPacks()) {
                if (scriptPack != resource)
                    continue;
                // scriptPack.reload(files);
                std::cout << "Reloaded script pack " << scriptPack.getName() << std::endl;
                for (auto &[name, klass] : scriptPack.getClasses()) {
                    this->_resourceManager->getScripts()[name].reload(scriptPack.getDomain(), klass, scriptPack.getCoreClass());

                    SparseArray &sparseArray = componentManager.getAllInstanceFields(name);
                    for (std::size_t i = 0; i < sparseArray.getSize(); i++) {
                        if (sparseArray.getValues(i).has_value()) {
                            std::unordered_map<std::string, std::any> fieldsValues;
                            for (auto &[fieldName, field] : this->_resourceManager->getScripts()[name].getFields()) {
                                if (field.type == "Single")
                                    fieldsValues[fieldName] = 0.0f;
                                if (field.type == "Entity" || field.type == "UInt64")
                                    fieldsValues[fieldName] = (unsigned long)0;
                            }
                            std::unordered_map<std::string, std::any> oldFieldsValues = std::any_cast<std::unordered_map<std::string, std::any> &>(sparseArray.getValues(i).value());
                            for (auto &[fieldName, oldFieldValue] : oldFieldsValues) {
                                if (fieldsValues.find(fieldName) != fieldsValues.end()) {
                                    fieldsValues[fieldName] = oldFieldValue;
                                }
                            }
                            componentManager.addInstanceFields(name, i, fieldsValues);
                        }
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        break;
    default:
        break;
    }
}
