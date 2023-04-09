#include "Systems/Game/GameSystem.hpp"

using namespace Lush;

GameSystem::GameSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(std::move(graphic)), _resourceManager(std::move(resourceManager))
{
    Shapes::setupPlane(this->_screen);
    this->_graphic->setRunning(true);
}

void GameSystem::update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime)
{
    if (!this->_started) {
        this->initInstances(entityManager, componentManager);
        this->_started = true;
    }
    if (this->_graphic->getPaused() || !this->_graphic->getRunning())
        return;
    if (!this->shouldUpdate(deltaTime))
        return;

    this->_graphic->setGameViewPort(glm::vec4(0, 0, this->_graphic->getWindowSize().x, this->_graphic->getWindowSize().y));
    this->_graphic->getRenderView().use("Game");
    glBindVertexArray(this->_screen.vao);
    glBindTexture(GL_TEXTURE_2D, this->_graphic->getFrameBuffer("render").texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GameSystem::initInstances(EntityManager &entityManager, ComponentManager &componentManager)
{
    std::size_t it = this->_resourceManager->getScripts().size() - 1;
    for (auto &[name, script] : this->_resourceManager->getScripts()) {
        for (auto id : entityManager.getMaskCategory(ComponentType::COMPONENT_TYPE_COUNT << it))
            this->_resourceManager->getInstances().emplace_back(script, id, componentManager.getInstanceFields(name, id));
        it--;
    }
    for (auto &instance : this->_resourceManager->getInstances())
        instance.init();
}
