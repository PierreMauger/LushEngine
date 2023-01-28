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
}
