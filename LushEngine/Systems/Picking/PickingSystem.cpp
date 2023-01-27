#include "Systems/Picking/PickingSystem.hpp"

using namespace Lush;

PickingSystem::PickingSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager) : _graphic(graphic)
{
    entityManager.addMaskCategory(MODEL_TAG);
    entityManager.addMaskCategory(BILLBOARD_TAG);
    glm::vec2 windowSize = this->_graphic->getWindowSize();

    glGenFramebuffers(1, &this->_buffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);
    glGenTextures(1, &this->_buffer.texture);
    glBindTexture(GL_TEXTURE_2D, this->_buffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_buffer.texture, 0);
    glGenRenderbuffers(1, &this->_buffer.depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->_buffer.depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSize.x, windowSize.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->_buffer.depthbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->_graphic->getFrameBuffers()["picking"] = this->_buffer;

    glGenVertexArrays(1, &this->_billboard.vao);
    glGenBuffers(1, &this->_billboard.vbo);
    glBindVertexArray(this->_billboard.vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->_billboard.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboardVertices), &billboardVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glGenVertexArrays(1, &this->_plane.vao);
    glGenBuffers(1, &this->_plane.vbo);
    glBindVertexArray(this->_plane.vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->_plane.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

void PickingSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);
    glm::vec4 viewport = this->_graphic->getSceneViewPort();
    glm::vec2 windowSize = this->_graphic->getWindowSize();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->_graphic->getRenderView().use("PickingCamera");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(MODEL_TAG)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Model model = componentManager.getComponent<Model>(id);

        glm::vec4 color;
        color.r = (((id + 1) & 0x000000FF) >> 0) / 255.0f;
        color.g = (((id + 1) & 0x0000FF00) >> 8) / 255.0f;
        color.b = (((id + 1) & 0x00FF0000) >> 16) / 255.0f;
        color.a = 1.0f;

        this->_graphic->getRenderView().getShader().setVec4("id", color);
        this->_graphic->getRenderView().setModel(transform);
        if (this->_graphic->getModels().find(model.name) != this->_graphic->getModels().end())
            this->_graphic->getModels()[model.name].draw(this->_graphic->getRenderView().getShader());
    }

    this->_graphic->getRenderView().use("PickingBillboard");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(BILLBOARD_TAG)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Billboard billboard = componentManager.getComponent<Billboard>(id);

        glm::vec4 color;
        color.r = (((id + 1) & 0x000000FF) >> 0) / 255.0f;
        color.g = (((id + 1) & 0x0000FF00) >> 8) / 255.0f;
        color.b = (((id + 1) & 0x00FF0000) >> 16) / 255.0f;
        color.a = 1.0f;

        this->_graphic->getRenderView().getShader().setVec4("id", color);
        this->_graphic->getRenderView().setBillboard(transform);
        glBindVertexArray(this->_billboard.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    glm::vec2 mousePosition = this->_graphic->getMousePosition();
    // convert from viewport coord to screen coord (picking buffer is drawn on whole screen and resized later to viewport)
    mousePosition.x = (mousePosition.x - viewport.x) * windowSize.x / viewport.z;
    mousePosition.y = (mousePosition.y - viewport.y) * windowSize.y / viewport.w;

    std::size_t pixel = 0;
    if (this->_graphic->getSceneMovement()) {
        glReadPixels(mousePosition.x, windowSize.y - mousePosition.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
        pixel = pixel & 0x00FFFFFF;
        this->_graphic->setHoveredEntity(pixel - 1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, this->_graphic->getFrameBuffers()["scene"].framebuffer);
    glEnable(GL_BLEND);
    this->_graphic->getRenderView().use("Outline");
    this->_graphic->getRenderView().getShader().setInt("id", pixel);
    glBindTexture(GL_TEXTURE_2D, this->_buffer.texture);
    glBindVertexArray(this->_plane.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
}
