#include "Systems/Picking/PickingSystem.hpp"

using namespace Lush;

PickingSystem::PickingSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;
    entityManager.addMaskCategory(this->_modelTag);
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
    this->_graphic->getFrameBuffers().push_back(this->_buffer);

    glGenVertexArrays(1, &this->_planeVAO);
    glGenBuffers(1, &this->_planeVBO);
    glBindVertexArray(this->_planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

void PickingSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);
    glm::vec4 viewport = this->_graphic->getGameViewPort();
    glm::vec2 windowSize = this->_graphic->getWindowSize();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->_graphic->getRenderView().use("Picking");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(this->_modelTag)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Model model = componentManager.getComponent<Model>(id);

        glm::vec4 color;
        color.r = (((id + 1) & 0x000000FF) >> 0) / 255.0f;
        color.g = (((id + 1) & 0x0000FF00) >> 8) / 255.0f;
        color.b = (((id + 1) & 0x00FF0000) >> 16) / 255.0f;
        color.a = 1.0f;

        this->_graphic->getShaders()["Picking"].setVec4("id", color);
        this->_graphic->getRenderView().setModel(transform);
        if (this->_graphic->getModels().find(model.id) != this->_graphic->getModels().end())
            this->_graphic->getModels()[model.id].draw(this->_graphic->getRenderView().getShader());
    }
    glm::vec2 mousePosition = this->_graphic->getMousePosition();
    unsigned char pixel[4] = {0};
    // convert from viewport coord to screen coord (picking buffer is drawn on whole screen and resize later to viewport)
    mousePosition.x = (mousePosition.x - viewport.x) * windowSize.x / viewport.z;
    mousePosition.y = (mousePosition.y - viewport.y) * windowSize.y / viewport.w;

    glReadPixels(mousePosition.x, windowSize.y - mousePosition.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, this->_graphic->getFrameBuffers()[0].framebuffer);
    this->_graphic->getRenderView().use("Outline");
    this->_graphic->getShaders()["Outline"].setInt("id", (pixel[0]) + (pixel[1] << 8) + (pixel[2] << 16));
    glBindTexture(GL_TEXTURE_2D, this->_buffer.texture);
    glBindVertexArray(this->_planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
