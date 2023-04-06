#include "Systems/Render/RenderSystem.hpp"
#include <utility>

using namespace Lush;

RenderSystem::RenderSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(std::move(graphic)), _resourceManager(std::move(resourceManager))
{
    Shapes::setupFrameBuffer(this->_buffer, this->_graphic->getWindowSize());
    this->_graphic->getFrameBuffers()["render"] = this->_buffer;

    Shapes::setupSkyBox(this->_skybox);
    Shapes::setupBillboard(this->_billboard);
}

RenderSystem::~RenderSystem()
{
    Shapes::deleteFrameBuffer(this->_buffer);
    Shapes::deleteBufferObject(this->_skybox);
    Shapes::deleteBufferObject(this->_billboard);
}

void RenderSystem::update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    this->_graphic->getRenderView().setAspectRatio(this->_graphic->getGameViewPort().z / this->_graphic->getGameViewPort().w);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->drawSkybox(entityManager, componentManager);
    this->drawMap(entityManager, componentManager);
    this->drawModels(entityManager, componentManager);
    this->drawBillboards(entityManager, componentManager);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::drawModels(EntityManager &entityManager, ComponentManager &componentManager)
{
    this->_graphic->getRenderView().use("Model");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(MODEL_TAG)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Model model = componentManager.getComponent<Model>(id);

        this->_graphic->getRenderView().setModel(transform);
        if (this->_resourceManager->getModels().find(model.name) != this->_resourceManager->getModels().end())
            this->_resourceManager->getModels()[model.name].draw(this->_graphic->getRenderView().getShader());
    }
}

void RenderSystem::drawBillboards(EntityManager &entityManager, ComponentManager &componentManager)
{
    this->_graphic->getRenderView().use("Billboard");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(BILLBOARD_TAG)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Billboard billboard = componentManager.getComponent<Billboard>(id);

        this->_graphic->getRenderView().setBillboard(transform);
        glActiveTexture(GL_TEXTURE0);
        if (this->_resourceManager->getTextures().find(billboard.name) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[billboard.name].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("tex", 0);
        glBindVertexArray(this->_billboard.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

void RenderSystem::drawMap(EntityManager &entityManager, ComponentManager &componentManager)
{
    this->_graphic->getRenderView().use("Map");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(MAP_TAG)) {
        Map map = componentManager.getComponent<Map>(id);

        this->_graphic->getRenderView().getShader().setMat4("model", glm::mat4(1.0f));
        glActiveTexture(GL_TEXTURE0);
        if (this->_resourceManager->getTextures().find(map.heightMap) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.heightMap].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("heightMap", 0);
        glActiveTexture(GL_TEXTURE1);
        if (this->_resourceManager->getTextures().find(map.diffuseTexture) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.diffuseTexture].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("diffuseTexture", 1);
        glActiveTexture(GL_TEXTURE2);
        if (this->_resourceManager->getTextures().find(map.normalTexture) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.normalTexture].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("normalTexture", 2);
        glActiveTexture(GL_TEXTURE3);
        if (this->_resourceManager->getTextures().find(map.diffuseTexture2) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.diffuseTexture2].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("diffuseTexture2", 3);
        glActiveTexture(GL_TEXTURE4);
        if (this->_resourceManager->getTextures().find(map.diffuseTexture3) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.diffuseTexture3].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("diffuseTexture3", 4);
        this->_resourceManager->getMap().draw();
    }
}

void RenderSystem::drawSkybox(EntityManager &entityManager, ComponentManager &componentManager)
{
    glDepthFunc(GL_LEQUAL);
    this->_graphic->getRenderView().use("Skybox");
    this->_graphic->getRenderView().setSkyBoxView();
    for (auto id : entityManager.getMaskCategory(SKYBOX_TAG)) {
        Cubemap cubeMap = componentManager.getComponent<Cubemap>(id);

        if (this->_resourceManager->getSkyboxes().find(cubeMap.name) != this->_resourceManager->getSkyboxes().end()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->_resourceManager->getSkyboxes()[cubeMap.name].getId());
            this->_graphic->getRenderView().getShader().setInt("skybox", 0);
            glBindVertexArray(this->_skybox.vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glDepthFunc(GL_LESS);
}
