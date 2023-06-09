#include "Systems/Script/ScriptSystem.hpp"

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(std::move(graphic)), _resourceManager(std::move(resourceManager))
{
    ScriptGlue::registerFunctions();
}

void ScriptSystem::update([[maybe_unused]] EntityManager &entityManager, float deltaTime)
{
    if (this->_graphic->getPaused() || !this->_graphic->getRunning())
        return;
    if (!this->shouldUpdate(deltaTime))
        return;
    for (auto &instance : this->_resourceManager->getInstances())
        instance.update(this->getDeltaTime());
}
