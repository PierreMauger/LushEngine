#include "Systems/FileWatcher/FileWatcherSystem.hpp"

using namespace Lush;

FileWatcherSystem::FileWatcherSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager) : ASystem(3.0f), _graphic(graphic)
{
}

FileWatcherSystem::~FileWatcherSystem()
{
}

void FileWatcherSystem::update(EntityManager &entityManager, [[maybe_unused]] ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    for (auto &[name, file] : this->_graphic->getFiles()) {
        if (file.isModified()) {
            file.update();
            std::cout << "File " << file.getPath() << " has been modified" << std::endl;
            for (auto &[name, res] : this->_graphic->getResources())
                if (res.hasFile(file))
                    res.reload();
        }
    }
}
