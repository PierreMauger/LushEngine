#include "Systems/Script/ScriptSystem.hpp"

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    ScriptGlue::registerFunctions();
}

void ScriptSystem::update([[maybe_unused]] std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    if (this->_graphic->isPaused() || !this->_graphic->isRunning() || !this->shouldUpdate(deltaTime))
        return;
    for (auto &instance : this->_resourceManager->getScriptInstances()) {
        instance.update(this->getDeltaTime());
        if (this->_resourceManager->isSceneChanged()) {
            this->_resourceManager->setSceneChanged(false);
            return;
        }
    }
}
