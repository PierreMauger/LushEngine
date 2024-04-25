#include "Systems/Picking/PickingSystem.hpp"

using namespace Lush;

PickingSystem::PickingSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(std::move(graphic)), _resourceManager(std::move(resourceManager))
{
    Shapes::setupFrameBuffer(this->_graphic->getFrameBuffers()["picking"], this->_graphic->getWindowSize());

    Shapes::setupBillboard(this->_billboard);
    Shapes::setupPlane(this->_plane);
}

PickingSystem::~PickingSystem()
{
    Shapes::deleteBufferObject(this->_billboard);
    Shapes::deleteBufferObject(this->_plane);
}

void PickingSystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    glm::vec4 viewport = this->_graphic->getSceneViewPort();
    glm::vec2 windowSize = this->_graphic->getWindowSize();
    glBindFramebuffer(GL_FRAMEBUFFER, this->_graphic->getFrameBuffers()["picking"].framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->_graphic->getRenderView().use("PickingModel");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (entity.getParent().has_value())
            continue;
        if (entity.hasComponent<Transform>() && entity.hasComponent<Model>())
            this->drawModel(entity, id, entityManager);
    }

    this->_graphic->getRenderView().use("PickingBillboard");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (entity.getParent().has_value())
            continue;
        if (entity.hasComponent<Transform>() && entity.hasComponent<Billboard>())
            this->drawBillboard(entity, id, entityManager);
    }

    // convert from viewport coord to screen coord (picking buffer is drawn on whole screen and resized later to viewport)
    glm::vec2 mousePosition = this->_graphic->getMousePosition();
    mousePosition.x = (mousePosition.x - viewport.x) * windowSize.x / viewport.z;
    mousePosition.y = (mousePosition.y - viewport.y) * windowSize.y / viewport.w;

    std::size_t pixel = 0;
    if (this->_graphic->isSceneHovered()) {
        glReadPixels(mousePosition.x, windowSize.y - mousePosition.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
        pixel = pixel & 0x00FFFFFF;
        this->_graphic->setHoveredEntity(pixel - 1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->drawOutline(pixel);
}

void PickingSystem::drawModel(Entity &entity, std::size_t id, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform)
{
    Model model = entity.getComponent<Model>();
    Transform transform = entity.getComponent<Transform>();
    transform.rotation = parentTransform.rotation * transform.rotation;
    transform.position = parentTransform.rotation * transform.position + parentTransform.position;
    glm::vec4 color((((id + 1) & 0x000000FF) >> 0) / 255.0f, (((id + 1) & 0x0000FF00) >> 8) / 255.0f, (((id + 1) & 0x00FF0000) >> 16) / 255.0f, 1.0f);

    this->_graphic->getRenderView().getShader().setVec4("id", color);
    this->_graphic->getRenderView().setModel(transform);
    if (this->_resourceManager->getModels().contains(model.name))
        this->_resourceManager->getModels()[model.name]->draw(this->_graphic->getRenderView().getShader(), model, this->_resourceManager->getTextures());

    for (auto &childId : entity.getChildren()) {
        if (!entityManager->getEntities().contains(childId))
            continue;
        Entity &child = entityManager->getEntities()[childId];
        if (child.hasComponent<Transform>() && child.hasComponent<Model>())
            this->drawModel(child, childId, entityManager, transform);
    }
}

void PickingSystem::drawBillboard(Entity &entity, std::size_t id, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform)
{
    Billboard billboard = entity.getComponent<Billboard>();
    Transform transform = entity.getComponent<Transform>();
    transform.rotation = parentTransform.rotation * transform.rotation;
    transform.position = parentTransform.rotation * transform.position + parentTransform.position;
    glm::vec4 color((((id + 1) & 0x000000FF) >> 0) / 255.0f, (((id + 1) & 0x0000FF00) >> 8) / 255.0f, (((id + 1) & 0x00FF0000) >> 16) / 255.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures().contains(billboard.name) ? this->_resourceManager->getTextures()[billboard.name]->getId() : 0);
    this->_graphic->getRenderView().getShader().setInt("tex", 0);
    this->_graphic->getRenderView().getShader().setVec4("id", color);
    this->_graphic->getRenderView().getShader().setBool("lockYAxis", billboard.lockYAxis);
    this->_graphic->getRenderView().setBillboard(transform);
    glBindVertexArray(this->_billboard.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    for (auto &childId : entity.getChildren()) {
        if (!entityManager->getEntities().contains(childId))
            continue;
        Entity &child = entityManager->getEntities()[childId];
        if (child.hasComponent<Transform>() && child.hasComponent<Model>())
            this->drawModel(child, childId, entityManager, transform);
    }
}

void PickingSystem::drawOutline(std::size_t pixel)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->_graphic->getFrameBuffers()["scene"].framebuffer);
    glEnable(GL_BLEND);
    this->_graphic->getRenderView().use("Outline");
    this->_graphic->getRenderView().getShader().setInt("id", (int)pixel);
    glBindTexture(GL_TEXTURE_2D, this->_graphic->getFrameBuffers()["picking"].texture);
    glBindVertexArray(this->_plane.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
}
