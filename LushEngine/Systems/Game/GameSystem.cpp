#include "Systems/Game/GameSystem.hpp"

using namespace Lush;

GameSystem::GameSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    Shapes::setupPlane(this->_screen);
    this->_graphic->setRunning(true);
}

void GameSystem::update(EntityManager &entityManager, float deltaTime)
{
    if (!this->_started) {
        this->initScriptInstances(entityManager);
        this->_started = true;
    }
    if (this->_graphic->getPaused() || !this->_graphic->getRunning() || !this->shouldUpdate(deltaTime))
        return;

    this->_graphic->setGameViewPort(glm::vec4(0, 0, this->_graphic->getWindowSize().x, this->_graphic->getWindowSize().y));
    this->_graphic->getRenderView().use("Game");
    glBindVertexArray(this->_screen.vao);
    glBindTexture(GL_TEXTURE_2D, this->_graphic->getFrameBuffer("render").texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GameSystem::initScriptInstances(EntityManager &entityManager)
{
    std::size_t it = this->_resourceManager->getScripts().size() - 1;
    for (auto &[name, script] : this->_resourceManager->getScripts()) {
        for (auto &[id, entity] : entityManager.getEntities())
            if (entity.hasScriptComponent(name))
                this->_resourceManager->getScriptInstances().emplace_back(script, id, entity.getScriptComponent(name).getFields());
        it--;
    }
    for (auto &instance : this->_resourceManager->getScriptInstances())
        instance.init();
}
