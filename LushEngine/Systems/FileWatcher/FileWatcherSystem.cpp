#include "Systems/FileWatcher/FileWatcherSystem.hpp"

using namespace Lush;

FileWatcherSystem::FileWatcherSystem(std::shared_ptr<Graphic> graphic, [[maybe_unused]] EntityManager &entityManager) : ASystem(3.0f), _graphic(graphic)
{
}

void FileWatcherSystem::update([[maybe_unused]] EntityManager &entityManager, [[maybe_unused]] ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    for (auto &[name, file] : this->_graphic->getFiles()) {
        if (file.isModified()) {
            file.update();
            this->reloadResourcesFromFile(file);
        }
    }
    if (!this->_graphic->getRunning() && !this->_resourcesToReload.empty()) {
        for (auto &res : this->_resourcesToReload)
            this->updateResource(res);
        this->_resourcesToReload.clear();
    }
}

void FileWatcherSystem::reloadResourcesFromFile(File &file)
{
    for (auto &res : Resource::getResources())
        if (res.hasFile(file)) {
            std::cout << "Reloading resource " << res.getUUID() << " for file " << file.getPath() << std::endl;
            this->updateResource(res);
        }
}

void FileWatcherSystem::updateResource(Resource &resource)
{
    std::vector<File> files = resource.getFiles();

    switch (resource.getType()) {
    case ResourceType::MODEL:
        for (auto &[name, model] : this->_graphic->getModels()) {
            if (model == resource) {
                try {
                    model.reload(files[0], this->_graphic->getTextures());
                    std::cout << "Reloaded model " << name << std::endl;
                } catch (const std::exception &e) {
                    std::cout << e.what() << std::endl;
                }
            }
        }
        break;
    case ResourceType::SHADER:
        for (auto &[name, shader] : this->_graphic->getShaders()) {
            if (shader == resource) {
                try {
                    if (files.size() == 2)
                        shader.reload(files[0], files[1]);
                    else if (files.size() == 3)
                        shader.reload(files[0], files[1], files[2]);
                    else if (files.size() == 4)
                        shader.reload(files[0], files[1], files[2], files[3]);
                    else if (files.size() == 5)
                        shader.reload(files[0], files[1], files[2], files[3], files[4]);
                    this->_graphic->getRenderView().setShaders(this->_graphic->getShaders());
                    std::cout << "Reloaded shader " << name << std::endl;
                } catch (const std::exception &e) {
                    std::cout << e.what() << std::endl;
                }
            }
        }
        break;
    case ResourceType::SCRIPT:
        for (auto &[name, script] : this->_graphic->getScripts()) {
            if (script == resource) {
                if (this->_graphic->getRunning()) {
                    this->_resourcesToReload.push_back(resource);
                    std::cout << "Scheduled reloading script " << name << std::endl;
                    break;
                }
                try {
                    script.reload(files[0]);
                    std::cout << "Reloaded script " << name << std::endl;
                } catch (const std::exception &e) {
                    std::cout << e.what() << std::endl;
                }
            }
        }
        break;
    default:
        break;
    }
}
