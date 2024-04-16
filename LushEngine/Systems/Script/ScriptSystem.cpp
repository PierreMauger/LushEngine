#include "Systems/Script/ScriptSystem.hpp"

#include <utility>

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(std::move(graphic)), _resourceManager(std::move(resourceManager))
{
    ScriptGlue::registerFunctions();
}

void ScriptSystem::update([[maybe_unused]] std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    if (this->_graphic->isPaused() || !this->_graphic->isRunning())
        return;
    for (auto it = this->_resourceManager->getScriptInstances().begin(); it != this->_resourceManager->getScriptInstances().end();) {
        auto &[id, instance] = *it;
        if (!entityManager->hasEntity(instance.getId())) {
            this->_resourceManager->getScriptInstances()[id].onDestroy();
            it = this->_resourceManager->getScriptInstances().erase(it);
            continue;
        }
        instance.onUpdate(deltaTime);
        if (this->_resourceManager->isSceneChanged()) {
            this->_resourceManager->setSceneChanged(false);
            return;
        }
        ++it;
    }
}
