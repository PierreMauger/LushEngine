#include "Systems/Render/RenderSystem.hpp"
#include <utility>

using namespace Lush;

RenderSystem::RenderSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    Shapes::setupFrameBuffer(this->_buffer, this->_graphic->getWindowSize());
    this->_graphic->getFrameBuffers()["render"] = this->_buffer;

    Shapes::setupSkybox(this->_skybox);
    Shapes::setupBillboard(this->_billboard);
}

RenderSystem::~RenderSystem()
{
    Shapes::deleteFrameBuffer(this->_buffer);
    Shapes::deleteBufferObject(this->_skybox);
    Shapes::deleteBufferObject(this->_billboard);
}

void RenderSystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    this->_graphic->getRenderView().setAspectRatio(this->_graphic->getGameViewPort().z / this->_graphic->getGameViewPort().w);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->drawSkybox(entityManager);
    this->drawMap(entityManager);
    this->drawModels(entityManager);
    this->drawBillboards(entityManager);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::drawModels(std::shared_ptr<EntityManager> &entityManager)
{
    this->_graphic->getRenderView().use("Model");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Model>())
            continue;
        Transform transform = entity.getComponent<Transform>();
        Model model = entity.getComponent<Model>();

        this->_graphic->getRenderView().setModel(transform);
        if (this->_resourceManager->getModels().find(model.name) != this->_resourceManager->getModels().end())
            this->_resourceManager->getModels()[model.name].draw(this->_graphic->getRenderView().getShader());
    }
}

void RenderSystem::drawBillboards(std::shared_ptr<EntityManager> &entityManager)
{
    this->_graphic->getRenderView().use("Billboard");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Billboard>())
            continue;
        Transform transform = entity.getComponent<Transform>();
        Billboard billboard = entity.getComponent<Billboard>();

        this->_graphic->getRenderView().setBillboard(transform);
        glActiveTexture(GL_TEXTURE0);
        if (this->_resourceManager->getTextures().find(billboard.name) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[billboard.name].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("tex", 0);
        this->_graphic->getRenderView().getShader().setBool("lockYAxis", billboard.lockYAxis);
        glBindVertexArray(this->_billboard.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

void RenderSystem::drawMap(std::shared_ptr<EntityManager> &entityManager)
{
    this->_graphic->getRenderView().use("Map");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Map>())
            continue;
        Map map = entity.getComponent<Map>();

        this->_graphic->getRenderView().setModel(entity.getComponent<Transform>());
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
        this->_resourceManager->getMapMesh().draw();
    }
}

void RenderSystem::drawSkybox(std::shared_ptr<EntityManager> &entityManager)
{
    glDepthFunc(GL_LEQUAL);
    this->_graphic->getRenderView().use("Skybox");
    this->_graphic->getRenderView().setSkyboxView();
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (!entity.hasComponent<Cubemap>())
            continue;
        Cubemap cubemap = entity.getComponent<Cubemap>();

        if (this->_resourceManager->getSkyboxes().find(cubemap.name) != this->_resourceManager->getSkyboxes().end()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->_resourceManager->getSkyboxes()[cubemap.name].getId());
            this->_graphic->getRenderView().getShader().setInt("skybox", 0);
            glBindVertexArray(this->_skybox.vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glDepthFunc(GL_LESS);
}
