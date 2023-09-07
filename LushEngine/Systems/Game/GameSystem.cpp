#include "Systems/Game/GameSystem.hpp"

using namespace Lush;

GameSystem::GameSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    Shapes::setupPlane(this->_screen);
    this->_graphic->setRunning(true);
}

void GameSystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    if (!this->_started) {
        this->_resourceManager->initScriptInstances(entityManager);
        this->_resourceManager->initPhysicInstances(entityManager);
        this->_started = true;
    }
    if (this->_graphic->isPaused() || !this->_graphic->isRunning() || !this->shouldUpdate(deltaTime))
        return;

    this->_graphic->setGameViewPort(glm::vec4(0, 0, this->_graphic->getWindowSize().x, this->_graphic->getWindowSize().y));
    this->_graphic->getRenderView().use("Game");
    glBindVertexArray(this->_screen.vao);
    glBindTexture(GL_TEXTURE_2D, this->_graphic->getFrameBuffers()["render"].texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
