#include "Systems/Picking/PickingSystem.hpp"

using namespace Lush;

PickingSystem::PickingSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    Shapes::setupFrameBuffer(this->_buffer, this->_graphic->getWindowSize());
    this->_graphic->getFrameBuffers()["picking"] = this->_buffer;

    Shapes::setupBillboard(this->_billboard);
    Shapes::setupPlane(this->_plane);
}

PickingSystem::~PickingSystem()
{
    Shapes::deleteFrameBuffer(this->_buffer);
    Shapes::deleteBufferObject(this->_billboard);
    Shapes::deleteBufferObject(this->_plane);
}

void PickingSystem::update(EntityManager &entityManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    glm::vec4 viewport = this->_graphic->getSceneViewPort();
    glm::vec2 windowSize = this->_graphic->getWindowSize();
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->drawModels(entityManager);
    this->drawBillboards(entityManager);

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

void PickingSystem::drawModels(EntityManager &entityManager)
{
    this->_graphic->getRenderView().use("PickingModel");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Model>())
            continue;
        Transform transform = entity.getComponent<Transform>();
        Model model = entity.getComponent<Model>();

        glm::vec4 color;
        color.r = (((id + 1) & 0x000000FF) >> 0) / 255.0f;
        color.g = (((id + 1) & 0x0000FF00) >> 8) / 255.0f;
        color.b = (((id + 1) & 0x00FF0000) >> 16) / 255.0f;
        color.a = 1.0f;

        this->_graphic->getRenderView().getShader().setVec4("id", color);
        this->_graphic->getRenderView().setModel(transform);
        if (this->_resourceManager->getModels().find(model.name) != this->_resourceManager->getModels().end())
            this->_resourceManager->getModels()[model.name].draw(this->_graphic->getRenderView().getShader());
    }
}

void PickingSystem::drawBillboards(EntityManager &entityManager)
{
    this->_graphic->getRenderView().use("PickingBillboard");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Billboard>())
            continue;
        Transform transform = entity.getComponent<Transform>();
        Billboard billboard = entity.getComponent<Billboard>();

        glm::vec4 color;
        color.r = (((id + 1) & 0x000000FF) >> 0) / 255.0f;
        color.g = (((id + 1) & 0x0000FF00) >> 8) / 255.0f;
        color.b = (((id + 1) & 0x00FF0000) >> 16) / 255.0f;
        color.a = 1.0f;

        this->_graphic->getRenderView().getShader().setVec4("id", color);
        this->_graphic->getRenderView().getShader().setBool("lockYAxis", billboard.lockYAxis);
        this->_graphic->getRenderView().setBillboard(transform);
        glBindVertexArray(this->_billboard.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

void PickingSystem::drawOutline(std::size_t pixel)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->_graphic->getFrameBuffers()["scene"].framebuffer);
    glEnable(GL_BLEND);
    this->_graphic->getRenderView().use("Outline");
    this->_graphic->getRenderView().getShader().setInt("id", (int)pixel);
    glBindTexture(GL_TEXTURE_2D, this->_buffer.texture);
    glBindVertexArray(this->_plane.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
}
