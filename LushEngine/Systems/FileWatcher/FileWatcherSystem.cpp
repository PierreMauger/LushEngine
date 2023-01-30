#include "Systems/FileWatcher/FileWatcherSystem.hpp"

using namespace Lush;

FileWatcherSystem::FileWatcherSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager) : ASystem(3.0f), _graphic(graphic)
{
}

FileWatcherSystem::~FileWatcherSystem()
{
}

void FileWatcherSystem::update([[maybe_unused]] EntityManager &entityManager, [[maybe_unused]] ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    for (auto &[name, file] : this->_graphic->getFiles()) {
        if (file.isModified()) {
            file.update();
            this->updateResource(file, name);
        }
    }
}

void FileWatcherSystem::updateResource(File &file, std::string name)
{
    std::vector<Lush::Resource> resources = Resource::getResources();
    for (auto &res : resources)
        if (res.hasFile(file)) {
            switch (res.getType()) {
            case ResourceType::MODEL:
                try {
                    this->_graphic->getModels()[name] = RenderModel(file, this->_graphic->getTextures());
                } catch (std::exception &e) {
                    std::cout << "Error while reloading model " << name << ": " << e.what() << std::endl;
                }
                Resource::getResources().erase(std::remove(Resource::getResources().begin(), Resource::getResources().end(), res), Resource::getResources().end());
                break;
            case ResourceType::SHADER:
                break;
            case ResourceType::SCRIPT:
                break;
            default:
                break;
            }
        }
}
